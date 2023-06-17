#pragma once

#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace entt
{
   typedef basic_view<SphereComponent, IDComponent> SphereView;
   typedef basic_view<MeshComponent  , IDComponent> MeshView;
}

class Renderer
{
public:
   struct Settings
   {
      bool Accumulate = true;
   };

   Renderer() = default;
   ~Renderer() = default;

   void Resize(uint32_t width, uint32_t height);
   void Render(Scene& scene, const Camera& camera);
   void ResetFrameIndex() { m_FrameIndex = 1; }

   std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
   Settings& GetSettings() { return m_Settings; }
private:
   struct HitPayload
   {
      float HitDistance;
      glm::vec3 WorldPos;
      glm::vec3 WorldNorm;
      uint64_t EntityUUID;
   };

   glm::vec4 PerPixel(uint32_t x, uint32_t y);
   HitPayload TraceRay(const Ray& ray);
   HitPayload Miss(const Ray& ray);
   HitPayload ReportIntersectionHit(float closestT, const Ray& ray, uint64_t entityUUID); // Custom hit "shader" for geometry other than triangles (Spheres)

   Scene* m_ActiveScene = nullptr;
   const Camera* m_ActiveCamera = nullptr;

   // A bit ugly to store the unpacked "entt::views" like this, but it might be decent for the cache anyways since we'll iterate these
   // Doing this for now because it's extremly slow to grab the views and the components for each pixel, so might aswell do it once per frame and store them for easy access
   std::vector<std::pair<const SphereComponent, const IDComponent>> m_SphereComponents;
   std::vector<std::pair<const MeshComponent, const IDComponent>> m_MeshComponents;

   Settings m_Settings = {};

   std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
   std::shared_ptr<Walnut::Image> m_FinalImage = nullptr;
   uint32_t* m_ImageData = nullptr;
   glm::vec4* m_AccumulationData = nullptr;

   uint32_t m_FrameIndex = 1;
};
