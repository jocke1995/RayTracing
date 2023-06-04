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
   for (uint32_t i = 0; i < (m_FinalImage->GetWidth() * m_FinalImage->GetHeight()); i++)
   {
      m_ImageData[i] = Walnut::Random::UInt(); // AGBR
      m_ImageData[i] |= 0xff000000;
   }

   m_FinalImage->SetData(m_ImageData);
}
