#pragma once

#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include <memory>

#include "Camera.h"
#include "Ray.h"

class Renderer
{
public:
   Renderer() = default;
   ~Renderer() = default;

   void Resize(uint32_t width, uint32_t height);
   void Render(const Camera& camera);

   std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
   glm::vec4 TraceRay(const Ray& ray);

   std::shared_ptr<Walnut::Image> m_FinalImage = nullptr;
   uint32_t* m_ImageData = nullptr;
};
