#pragma once

#include "glm/glm.hpp"

#include <vector>
struct Sphere
{
   glm::vec3 Position {0.0f};
   float radius = 0.5f;

   glm::vec3 Albedo {1.0f};
};

class Scene
{
public:
   std::vector<Sphere> m_Spheres;
};

