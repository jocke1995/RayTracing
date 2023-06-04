#include "Renderer.h"

#include "Walnut/Random.h"

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

void Renderer::Render()
{
   // Render something
   for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
   {
      for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
      {
         glm::vec2 uv = { (float)x / m_FinalImage->GetWidth(), (float)y / m_FinalImage->GetHeight() };
         uint32_t imageDataIndex = x + (y * m_FinalImage->GetWidth());
         m_ImageData[imageDataIndex] = PerPixel(uv);
      }
   }

   m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 uv)
{
   glm::vec2 ndc = (uv * 2.0f) - 1.0f;

   glm::vec3 rayDir(ndc.x, ndc.y, -1.0f);
   rayDir = glm::normalize(rayDir);
   glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);

   float sphereRadius = 0.5f;

   // [Spheres] Solve for this eq
   // (b_x^2 + b_y^2 + b_z^2)t^2 + (2(a_x*b_x + a_y*b_y + a_z*b_z*))t + (a_x^2 + a_y^2 + a_z^2 - r^2) = 0
   //          (A)t^2          +            (B)t^2            +           C = 0 (in the quadratic formula)
   // a = ray origin
   // b = ray direction
   // r = radius
   // t = hit distance

   // A, B, C is part of the quadratic equation [(-B +- sqrt(B^2 - 4AC) / 2A]
   float A = glm::dot(rayDir, rayDir);
   float B = 2.0f * glm::dot(rayOrigin, rayDir);
   float C = glm::dot(rayOrigin, rayOrigin) - (sphereRadius * sphereRadius);

   // Discriminant = [B^2 - 4AC]
   float discriminant = (B * B) - (4 * A * C);

   if (discriminant >= 0.0f)
   {
      // We got a hit
      uint8_t red = (uint8_t)(uv.x * 255.0f);
      uint8_t green = (uint8_t)(uv.y * 255.0f);
      uint32_t color = 0xff000000 | (green << 8) | (red);
      return color;
   }
   
   return 0xff000000;
}
