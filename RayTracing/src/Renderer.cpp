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
   uint8_t red    = (uint8_t)(uv.x * 255.0f);
   uint8_t green  = (uint8_t)(uv.y * 255.0f);

   return 0xff000000 | (green << 8) | (red);
}
