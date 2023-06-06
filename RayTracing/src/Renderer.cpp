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

void Renderer::Render(const Camera& camera)
{
   Ray ray;
   ray.Origin = camera.GetPosition();

   // Render something
   for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
   {
      for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
      {
         uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());

         ray.Direction = camera.GetRayDirections()[imageDataIndex];
         glm::vec4 color = TraceRay(ray);
         color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

         // Write out the color
         m_ImageData[imageDataIndex] = Utils::ConvertToRGBA(color);
      }
   }

   m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
   float sphereRadius = 0.5f;

   // [Spheres] Solve for this eq
   // (b_x^2 + b_y^2 + b_z^2)t^2 + (2(a_x*b_x + a_y*b_y + a_z*b_z*))t + (a_x^2 + a_y^2 + a_z^2 - r^2) = 0
   //          (A)t^2          +            (B)t^2            +           C = 0 (in the quadratic formula)
   // a = ray origin
   // b = ray direction
   // r = radius
   // t = hit distance

   // A, B, C is part of the quadratic equation [(-B +- sqrt(B^2 - 4AC) / 2A]
   float A = glm::dot(ray.Direction, ray.Direction);
   float B = 2.0f * glm::dot(ray.Origin, ray.Direction);
   float C = glm::dot(ray.Origin, ray.Origin) - (sphereRadius * sphereRadius);

   // Discriminant = [B^2 - 4AC]
   float discriminant = (B * B) - (4 * A * C);

   if (discriminant < 0.0f)
   {
      // No hit, return skycolor
      return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
   }

   float t[2] =
   {
      (-B + glm::sqrt(discriminant)) / (2 * A),
      (-B - glm::sqrt(discriminant)) / (2 * A)
   };

   // no need for the other hit as t[1] will be closest
   float closestT = t[1];

   glm::vec3 hitPosition = ray.Origin + ray.Direction * closestT;

   glm::vec3 spherePos = {};
   glm::vec3 sphereNormal = hitPosition - spherePos;

   glm::vec3 lightPos = glm::vec3(-1.0f, -1.0f, -1.0f);

   float NdotV = glm::max(glm::dot(sphereNormal, -lightPos), 0.0f);

   return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) * NdotV;
}
