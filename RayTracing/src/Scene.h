#pragma once

#include "glm/glm.hpp"

#include <vector>

struct Material
{
   glm::vec3 Albedo{ 1.0f };
   float Roughness = 1.0f;
   float Metallic = 0.0f;

   glm::vec3 EmissionColor{ 0.0f };
   float EmissionPower = 0.0f;

   glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
};

struct Sphere
{
   glm::vec3 Position {0.0f};
   float Radius = 0.5f;

   int MaterialIndex = -1;
};

struct Vertex
{
   glm::vec3 Position;
   glm::vec3 Normal;
};

struct Mesh
{
   Vertex* Vertices = nullptr;
};

class Scene
{
public:
   std::vector<Sphere> m_Spheres;
   std::vector<Material> m_Materials;
   std::vector<Mesh> m_Meshes;
};

