#include "Renderer.h"

#include "Walnut/Random.h"

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
      m_FinalImage->Resize(width, height);
   }
   else
   {
      m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA); // 4 bytes per pixel
   }

   delete[] m_ImageData;
   m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
   m_ActiveScene = &scene;
   m_ActiveCamera = &camera;


   // Render something
   for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
   {
      for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
      {
         glm::vec4 color = PerPixel(x, y);
         color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

         // Write out the color
         uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());
         m_ImageData[imageDataIndex] = Utils::ConvertToRGBA(color);
      }
   }

   m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
   uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());

   Ray ray;
   ray.Origin = m_ActiveCamera->GetPosition();
   ray.Direction = m_ActiveCamera->GetRayDirections()[imageDataIndex];

   float multiplier = 1.0f;

   uint32_t numBounces = 1;
   glm::vec3 accumulatedColor{ 0.0f };
   for (uint32_t i = 0; i < numBounces; i++)
   {
      HitPayload payload = TraceRay(ray);

      if (payload.HitDistance < 0)
      {
         glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
         accumulatedColor += (skyColor * multiplier);
         continue;
      }

      const Sphere& sphere = m_ActiveScene->m_Spheres[payload.ObjectIndex];
      glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
      float NdotL = glm::max(glm::dot(payload.WorldNorm, -lightDir), 0.0f);
      glm::vec3 albedo = sphere.Albedo * NdotL;

      accumulatedColor += (albedo * multiplier);
      multiplier *= 0.7f;

      // Prepare for next iteration
      ray.Origin = payload.WorldPos + (payload.WorldNorm * 0.001f);
      ray.Direction = glm::reflect(ray.Direction, payload.WorldNorm);
   }
   accumulatedColor /= numBounces;

   return glm::vec4(accumulatedColor, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
   HitPayload payload;

   if (m_ActiveScene->m_Spheres.size() == 0)
   {
      payload.HitDistance = -1;
      return payload;
   }

   int closestSphere = -1;
   float hitDistance = FLT_MAX;

   for (uint32_t i = 0; i < m_ActiveScene->m_Spheres.size(); i++)
   {
      const Sphere& sphere = m_ActiveScene->m_Spheres[i];
      glm::vec3 origin = ray.Origin - sphere.Position;

      // [Spheres] Solve for this eq
      // (b_x^2 + b_y^2 + b_z^2)t^2 + (2(a_x*b_x + a_y*b_y + a_z*b_z*))t + (a_x^2 + a_y^2 + a_z^2 - r^2) = 0
      //          (A)t^2          +            (B)t^2            +           C = 0 (in the quadratic formula)
      // a = ray origin
      // b = ray direction
      // r = radius
      // t = hit distance

      // A, B, C is part of the quadratic equation [(-B +- sqrt(B^2 - 4AC) / 2A]
      glm::vec3 sphereToRayOrigin = ray.Origin - sphere.Position;
      float A = glm::dot(ray.Direction, ray.Direction);
      float B = 2.0f * glm::dot(sphereToRayOrigin, ray.Direction);
      float C = glm::dot(sphereToRayOrigin, sphereToRayOrigin) - (sphere.radius * sphere.radius);

      // Discriminant = [B^2 - 4AC]
      float discriminant = (B * B) - (4 * A * C);

      if (discriminant < 0.0f)
      {
         // No hit, check the next sphere
         continue;
      }

      float t[2] =
      {
         (-B + glm::sqrt(discriminant)) / (2 * A),
         (-B - glm::sqrt(discriminant)) / (2 * A)
      };

      // no need for the other hit as t[1] will be closest
      float closestT = t[1];

      if ((closestT < hitDistance) && (closestT >= 0.0f))
      {
         hitDistance = closestT;
         closestSphere = i;
      }
   }

   // No hit
   if (closestSphere == -1)
   {
      return Miss(ray);
   }

   return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
   HitPayload payload;

   const Sphere& closestSphere = m_ActiveScene->m_Spheres[objectIndex];
   payload.WorldPos = ray.Origin + ray.Direction * hitDistance;
   payload.WorldNorm = glm::normalize(payload.WorldPos - closestSphere.Position);
   payload.HitDistance = hitDistance;
   payload.ObjectIndex = objectIndex;

   return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
   HitPayload payload;
   payload.HitDistance = -1;
   return payload;
}


