#include "RayTracingHelper.h"

#include "glm/glm.hpp"
#include <glm/gtx/compatibility.hpp>

bool IsRayBehindTriangle(const glm::vec3& rayOrigin, const glm::vec3& triangleVertexPosition, const glm::vec3& triangleNormal)
{
   glm::vec3 rayToTriangle = triangleVertexPosition - rayOrigin;
   float dotProduct = glm::dot(rayToTriangle, triangleNormal);

   bool isBehind = dotProduct > 0.0f;
   return isBehind;
}

float RayTracingHelper::RayTriangleIntersection(const Ray& ray, const Vertex triangle[])
{
   Vertex v0 = triangle[0];
   Vertex v1 = triangle[1];
   Vertex v2 = triangle[2];

   glm::vec3 normal = v0.Normal; // They all have the same normal for now

   // If we're perpendicular to the triangle normal, we won't intersect
   // Also if we're behind the triangle, we want to backface cull it
   if (IsPerpendicular(ray.Direction, normal) || IsRayBehindTriangle(ray.Origin, v0.Position, normal))
   {
      return -1.0f;
   }

   // Find plane D
   float D = -glm::dot(normal, v0.Position);

   // P = O +tR (R is direction in this case to not confuse it with D from the plane equation [Ax + Bx + Cz + D = 0]
   // Substituting P for the point in the plane ens up with [A*Px + B*Py + C*Pz + D = 0].
   // Then subsituting the ray [O + tR] for [P] in that and we get this after solving for t: [t = -(N(A,B,C) * O + D) / N(A,B,C) * R]
   float t = -(glm::dot(normal, ray.Origin) + D) / glm::dot(normal, ray.Direction);

   // We might be behind the camera
   if (t < 0)
   {
      return -1.0f;
   }

   glm::vec3 hitPos = ray.Origin + t * ray.Direction;

   // At this point we know we hit the plane.
   // Next up is to figure out if we're inside the triangle in the plane
   // We can do that by checking that the point is to the left of each edge in the triangle.
   // by taking the dot product between the normal and the cross(edge0, v0) and checking if it's greater then 0
   // Check triangle edges to see if 
   {
      glm::vec3 edge0 = v1.Position - v0.Position;
      glm::vec3 edge1 = v2.Position - v1.Position;
      glm::vec3 edge2 = v0.Position - v2.Position;
      glm::vec3 C0 = hitPos - v0.Position;
      glm::vec3 C1 = hitPos - v1.Position;
      glm::vec3 C2 = hitPos - v2.Position;

      if (  glm::dot(normal, glm::cross(edge0, C0)) > 0 &&
            glm::dot(normal, glm::cross(edge1, C1)) > 0 &&
            glm::dot(normal, glm::cross(edge2, C2)) > 0)
      {
         // P is inside the triangle
         return t; 
      }
   }
   
   return -1.0f;
}

bool RayTracingHelper::IsPerpendicular(glm::vec3 vec0, glm::vec3 vec1)
{
   const float epsilon = 0.001f;

   float dotProduct = glm::dot(glm::normalize(vec0), glm::normalize(vec1));
   bool isEqual = glm::epsilonEqual(dotProduct, 0.0f, epsilon);

   return isEqual;
}
