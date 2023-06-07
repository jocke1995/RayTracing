#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

Camera::Camera(float verticalFOV, float nearClip, float farClip)
   : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
{
   m_ForwardDirection = glm::vec3(0, 0, -1);
   m_Position = glm::vec3(0, 0, 3);
}

bool Camera::Update(float ts)
{
   static const float sensitivity = 0.002f;
   glm::vec2 mousePos = Walnut::Input::GetMousePosition();
   glm::vec2 mouseDelta = (mousePos - m_LastMousePosition) * sensitivity;
   m_LastMousePosition = mousePos;

   if (not Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right))
   {
      // If we're not moving the camera, we just stop here
      Walnut::Input::SetCursorMode(Walnut::CursorMode::Normal);
      return false;
   }

   Walnut::Input::SetCursorMode(Walnut::CursorMode::Locked);

   constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
   glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);

   float speed = 5.0f;
   bool moved = false;

   // Movement
   if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
   {
      m_Position += m_ForwardDirection * speed * ts;
      moved = true;
   }
   else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
   {
      m_Position -= m_ForwardDirection * speed * ts;
      moved = true;
   }

   if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
   {
      m_Position -= rightDirection * speed * ts;
      moved = true;
   }
   else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
   {
      m_Position += rightDirection * speed * ts;
      moved = true;
   }

   if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Q))
   {
      m_Position -= upDirection * speed * ts;
      moved = true;
   }
   else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::E))
   {
      m_Position += upDirection * speed * ts;
      moved = true;
   }

   // Rotation
   if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
   {
      float pitchDelta = mouseDelta.y * GetRotationSpeed();
      float yawDelta = mouseDelta.x * GetRotationSpeed();

      // Build quaternion for the forward direction using the right/up directions with corresponding pitch/yaws
      glm::quat q = glm::normalize(glm::cross(
                        glm::angleAxis(-pitchDelta, rightDirection),
                        glm::angleAxis(-yawDelta, upDirection)
      ));

      m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

      moved = true;
   }

   if (moved)
   {
      RecalculateView();
      RecalculateRayDirections();
   }

   return moved;
}

void Camera::Resize(uint32_t width, uint32_t height)
{
   if (width == m_ViewportWidth && height == m_ViewportHeight)
   {
      return;
   }

   m_ViewportWidth = width;
   m_ViewportHeight = height;

   RecalculateProjection();
   RecalculateRayDirections();
}

float Camera::GetRotationSpeed()
{
   return 0.5f;
}

void Camera::RecalculateProjection()
{
   m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
   m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView()
{
   m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
   m_InverseView = glm::inverse(m_View);
}

void Camera::RecalculateRayDirections()
{
   m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

   for (uint32_t y = 0; y < m_ViewportHeight; y++)
   {
      for (uint32_t x = 0; x < m_ViewportWidth; x++)
      {
         // From UV to world space [UV -> NDC -> Clipspace -> Viewspace -> Worldspace]
         glm::vec2 uv = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight }; // UV [0, 1]

         // UV -> NDC
         glm::vec2 ndc = uv * 2.0f - 1.0f; // NDC [-1, 1]

         // NDC -> Clipspace
         glm::vec4 clipCoord = m_InverseProjection * glm::vec4(ndc.x, ndc.y, 1, 1);

         // Clipspace -> Viewspace
         glm::vec4 viewDirection = glm::vec4(glm::vec3(clipCoord) / clipCoord.w, 0.0f); // Perspective division
         viewDirection = glm::normalize(viewDirection);

         // Viewspace -> Worldspace
         glm::vec3 rayDirection = glm::vec3(m_InverseView * viewDirection);

         m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
      }
   }
}
