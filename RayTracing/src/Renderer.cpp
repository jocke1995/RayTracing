#include "Renderer.h"

#include "AppRandom.h"
#include "RayTracingHelper.h"

#include <execution>

#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace Utils
{
   static uint32_t ConvertToRGBA(const glm::vec4& color)
   {
      uint8_t r = (int8_t)(color.r * 255.0f);
      uint8_t g = (int8_t)(color.g * 255.0f);
      uint8_t b = (int8_t)(color.b * 255.0f);
      uint8_t a = (int8_t)(color.a * 255.0f);

      uint32_t result = ((a << 24) | (b << 16) | (g << 8) | (r << 0));
      return result;
   }
}

void Renderer::Resize(uint32_t width, uint32_t height)
{
   if (m_FinalImage != nullptr)
   {
      if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
      {
         return;
      }

      m_FinalImage->Resize(width, height);
   }
   else
   {
      m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA); // 4 bytes per pixel
   }

   delete[] m_AccumulationData;
   m_AccumulationData = new glm::vec4[width * height];
   m_FrameIndex = 1;

   delete[] m_ImageData;
   m_ImageData = new uint32_t[width * height];

   m_ImageHorizontalIter.resize(width);
   m_ImageVerticalIter.resize(height);

   for (uint32_t i = 0; i < width; i++)
   {
      m_ImageHorizontalIter[i] = i;
   }

   for (uint32_t i = 0; i < height; i++)
   {
      m_ImageVerticalIter[i] = i;
   }
}

void Renderer::Render(Scene& scene, const Camera& camera)
{
   m_ActiveScene = &scene;
   m_ActiveCamera = &camera;

   // Package the entities we need nicely in an array for easy access
   {
      const auto& sphereView = m_ActiveScene->GetAllEntitiesWith<SphereComponent, IDComponent>();
      m_SphereComponents.clear();
      for (auto& entity : sphereView)
      {
         auto [sphere, id] = sphereView.get<SphereComponent, IDComponent>(entity);
         m_SphereComponents.push_back(std::make_pair(sphere, id));
      }

      const auto& meshView = m_ActiveScene->GetAllEntitiesWith<MeshComponent, IDComponent>();
      m_MeshComponents.clear();
      for (auto& entity : meshView)
      {
         auto [mesh, id] = meshView.get<MeshComponent, IDComponent>(entity);
         m_MeshComponents.push_back(std::make_pair(mesh, id));
      }
   }

   if (m_FrameIndex == 1)
   {
      memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
   }

#define MT
#if defined(MT)
   std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
      [this](uint32_t y) 
      {
         for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
         {
            uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());

            glm::vec4 color = PerPixel(x, y);
            m_AccumulationData[imageDataIndex] += color;

            glm::vec4 accumulatedColor = m_AccumulationData[imageDataIndex];
            accumulatedColor /= (float)m_FrameIndex;

            // Write out the color
            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[imageDataIndex] = Utils::ConvertToRGBA(accumulatedColor);
         }
      });
#else
   for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
   {
      for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
      {
         uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());

         glm::vec4 color = PerPixel(x, y);
         m_AccumulationData[imageDataIndex] += color;

         glm::vec4 accumulatedColor = m_AccumulationData[imageDataIndex];
         accumulatedColor /= (float)m_FrameIndex;

         // Write out the color
         accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
         m_ImageData[imageDataIndex] = Utils::ConvertToRGBA(accumulatedColor);
      }
   }
#endif
   m_FinalImage->SetData(m_ImageData);

   if (m_Settings.Accumulate == true)
   {
      m_FrameIndex++;
   }
   else
   {
      m_FrameIndex = 1;
   }
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
   uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());
   
   Ray ray;
   ray.Origin = m_ActiveCamera->GetPosition();
   ray.Direction = m_ActiveCamera->GetRayDirections()[imageDataIndex];
   
   glm::vec3 contribution { 1.0f };
   glm::vec3 accumulatedLight{ 0.0f };
   
   uint32_t numBounces = 5;
   for (uint32_t i = 0; i < numBounces; i++)
   {
      HitPayload payload = TraceRay(ray);
   
      if ((payload.HitDistance < 0) || (payload.EntityUUID == 0))
      {
         continue;
      }
   
      Entity entity = m_ActiveScene->GetEntityByUUID(payload.EntityUUID);
      if (entity.HasComponent<MaterialComponent>())
      {
         const MaterialComponent& materialComponent = entity.GetComponent<MaterialComponent>();
   
         Material& mat = *materialComponent.m_Material;
   
         accumulatedLight += (mat.m_Albedo * contribution);
         contribution *= mat.m_Albedo;
         accumulatedLight += (mat.GetEmission());
      }
   
      // Prepare for next iteration
      ray.Origin = payload.WorldPos + (payload.WorldNorm * 0.001f);
      ray.Direction = glm::normalize(payload.WorldNorm + AppRandom::InUnitSphere());
   }
   accumulatedLight /= numBounces;
   
   return glm::vec4(accumulatedLight, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
   HitPayload payload;
   payload.EntityUUID = 0;
   payload.HitDistance = -1;
   if (m_SphereComponents.empty() && m_MeshComponents.empty())
   {
      return payload;
   }

   float closestIntersectionHit = FLT_MAX;

   for (auto[sphereComponent, idComponent] : m_SphereComponents)
   {
      float closestT = RayTracingHelper::RaySphereIntersection(ray, sphereComponent.m_Position, sphereComponent.m_Radius);
      
      if ((closestT < closestIntersectionHit) && (closestT >= 0.0f))
      {
         closestIntersectionHit = closestT;
         payload.EntityUUID = idComponent.m_UUID;
      }
   }

   // Check if we hit anything with the "intersection shader"
   if (closestIntersectionHit != FLT_MAX)
   {
      payload = ReportIntersectionHit(closestIntersectionHit, ray, payload.EntityUUID);
   }

   float closestTriangleHit = FLT_MAX;
   for (auto [meshComponent, idComponent] : m_MeshComponents)
   {
      float closestT = RayTracingHelper::RayTriangleIntersection(ray, meshComponent.m_Mesh->m_Vertices);
   
      if ((closestT < closestTriangleHit) && (closestT >= 0.0f))
      {
         closestTriangleHit = closestT;
         payload.EntityUUID = idComponent.m_UUID;
      }
   }

   // Check if we hit anything with the "triangle-tracing shader"
   if (closestTriangleHit != FLT_MAX)
   {
      // Check whichever is closer, the intersectionGeometry or a triangle
      if (closestTriangleHit < closestIntersectionHit)
      {
         // We hit the triangle. Overwrite the payload with the triangle hit info
         payload.HitDistance = closestTriangleHit;

         // Calculate worldPos and normals;
         payload.WorldPos = {};
         payload.WorldNorm = {};
      }
   }

   // No hit
   if (payload.HitDistance == -1)
   {
      return Miss(ray);
   }

   return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
   HitPayload payload;
   payload.HitDistance = -1;
   return payload;
}

Renderer::HitPayload Renderer::ReportIntersectionHit(float closestT, const Ray& ray, uint64_t entityUUID)
{
   Entity entity = m_ActiveScene->GetEntityByUUID(entityUUID);
   
   HitPayload payload;
   payload.HitDistance = closestT;
   payload.EntityUUID = entityUUID;
   // Currently the only "intersection shader" geometry we got besides triangles.
   // Later we could add support for other customs types here, like metaballs, planes etc etc
   // So right now there is no pointin testing if we have a sphere, as we wouldn't get in here if we didn't
   // And apparently entt seems to be slow when checking this for some reason..
   // if (entity.HasComponent<SphereComponent>())
   {
      SphereComponent sphereComponent = entity.GetComponent<SphereComponent>();

      payload.WorldPos = ray.Origin + ray.Direction * closestT;
      payload.WorldNorm = glm::normalize(payload.WorldPos - sphereComponent.m_Position);
   }

   return payload;
}