#pragma once

#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
   Renderer() = default;
   ~Renderer() = default;

   void Resize(uint32_t width, uint32_t height);
   void Render(const Scene& scene, const Camera& camera);

   std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
   struct HitPayload
   {
      float HitDistance;
      glm::vec3 WorldPos;
      glm::vec3 WorldNorm;

      uint32_t ObjectIndex;
   };

   glm::vec4 PerPixel(uint32_t x, uint32_t y);
   HitPayload TraceRay(const Ray& ray);
   HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
   HitPayload Miss(const Ray& ray);

   const Scene* m_ActiveScene = nullptr;
   const Camera* m_ActiveCamera = nullptr;

   std::shared_ptr<Walnut::Image> m_FinalImage = nullptr;
   uint32_t* m_ImageData = nullptr;
};
