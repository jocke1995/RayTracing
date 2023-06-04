#pragma once

#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include <memory>

class Renderer
{
public:
   Renderer() = default;
   ~Renderer() = default;

   void Resize(uint32_t width, uint32_t height);
   void Render();

   std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
   uint32_t PerPixel(glm::vec2 uv);

   std::shared_ptr<Walnut::Image> m_FinalImage = nullptr;
   uint32_t* m_ImageData = nullptr;
};
