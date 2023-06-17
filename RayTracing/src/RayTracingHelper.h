#pragma once

#include "Ray.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"

class RayTracingHelper
{
public:
   // Returns -1 on miss, otherwise returns T
   static float RayTriangleIntersection(const Ray& ray, const Vertex triangle[]);
   static float RaySphereIntersection(const Ray& ray, glm::vec3 position, float radius);

   static bool IsPerpendicular(glm::vec3 vec0, glm::vec3 vec1);
private:

};