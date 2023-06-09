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

   glm::vec3 normal = v0.m_Normal; // They all have the same normal for now

   // If we're perpendicular to the triangle normal, we won't intersect
   // Also if we're behind the triangle, we want to backface cull it
   if (IsPerpendicular(ray.Direction, normal) || IsRayBehindTriangle(ray.Origin, v0.m_Position, normal))
   {
      return -1.0f;
   }

   // Find plane D
   float D = -glm::dot(normal, v0.m_Position);

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
      glm::vec3 edge0 = v1.m_Position - v0.m_Position;
      glm::vec3 edge1 = v2.m_Position - v1.m_Position;
      glm::vec3 edge2 = v0.m_Position - v2.m_Position;
      glm::vec3 C0 = hitPos - v0.m_Position;
      glm::vec3 C1 = hitPos - v1.m_Position;
      glm::vec3 C2 = hitPos - v2.m_Position;

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

float RayTracingHelper::RaySphereIntersection(const Ray& ray, glm::vec3 position, float radius)
{
   glm::vec3 origin = ray.Origin - position;

   // [Spheres] Solve for this eq
   // (b_x^2 + b_y^2 + b_z^2)t^2 + (2(a_x*b_x + a_y*b_y + a_z*b_z*))t + (a_x^2 + a_y^2 + a_z^2 - r^2) = 0
   //          (A)t^2          +            (B)t^2            +           C = 0 (in the quadratic formula)
   // a = ray origin
   // b = ray direction
   // r = radius
   // t = hit distance

   // A, B, C is part of the quadratic equation [(-B +- sqrt(B^2 - 4AC) / 2A]
   glm::vec3 sphereToRayOrigin = ray.Origin - position;
   float A = glm::dot(ray.Direction, ray.Direction);
   float B = 2.0f * glm::dot(sphereToRayOrigin, ray.Direction);
   float C = glm::dot(sphereToRayOrigin, sphereToRayOrigin) - (radius * radius);

   // Discriminant = [B^2 - 4AC]
   float discriminant = (B * B) - (4 * A * C);

   if (discriminant < 0.0f)
   {
      // No hit, check the next sphere
      return -1.0f;
   }

   //float t[2] =
   //{
   //   (-B + glm::sqrt(discriminant)) / (2 * A),
   //   (-B - glm::sqrt(discriminant)) / (2 * A)
   //};

   // no need for the other hit as the negative one will be closest
   float closestT = (-B - glm::sqrt(discriminant)) / (2 * A);

   return closestT;
}

bool RayTracingHelper::IsPerpendicular(glm::vec3 vec0, glm::vec3 vec1)
{
   const float epsilon = 0.001f;

   float dotProduct = glm::dot(glm::normalize(vec0), glm::normalize(vec1));
   bool isEqual = glm::epsilonEqual(dotProduct, 0.0f, epsilon);

   return isEqual;
}
