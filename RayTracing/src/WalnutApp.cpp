#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "glm/gtc/type_ptr.hpp"

// Misc Walnut
#include "Walnut/Timer.h"

// Raytracing specific
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:

   ExampleLayer()
      : m_Camera(45.0f, 0.1f, 100.0f)
   {
      {
         Material purpleMat;
         purpleMat.Albedo = { 1.0f, 0.0f, 1.0f };
         purpleMat.Roughness = 0.0f;
         purpleMat.Metallic = 0.0f;
         m_Scene.m_Materials.push_back(purpleMat);
      }

      {
         Material greenMat;
         greenMat.Albedo = { 0.0f, 1.0f, 0.0f };
         greenMat.Roughness = 0.0f;
         greenMat.Metallic = 1.0f;
         m_Scene.m_Materials.push_back(greenMat);
      }

      {
         Sphere sphere;
         sphere.Position = { 0.0f, 0.0f, 0.0f };
         sphere.Radius = 1.0f;
         sphere.MaterialIndex = 0;
         m_Scene.m_Spheres.push_back(sphere);
      }

      {
         Sphere sphere;
         sphere.Position = { 0.0f, -101.f, 0.0f };
         sphere.Radius = 100.0f;
         sphere.MaterialIndex = 1;
         m_Scene.m_Spheres.push_back(sphere);
      }
   };

   virtual void OnUpdate(float ts) override
   {
      m_Camera.Update(ts);
   }

   virtual void OnUIRender() override
   {
      ImGui::Begin("Settings");
      ImGui::Text("Last render: %.3fms", m_LastRenderTime);
      ImGui::End();

      ImGui::Begin("Scene");
      for (size_t i = 0; i < m_Scene.m_Spheres.size(); i++)
      {
         ImGui::PushID(i);

         Sphere& sphere = m_Scene.m_Spheres[i];
         ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
         ImGui::DragFloat("Radius", &(sphere.Radius), 0.1f);
         ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.m_Materials.size() - 1);

         ImGui::Separator();
         ImGui::PopID();
      }

      for (size_t i = 0; i < m_Scene.m_Materials.size(); i++)
      {
         ImGui::PushID(i);

         Material& material = m_Scene.m_Materials[i];
         ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo), 0.1f);
         ImGui::DragFloat("Roughness", &(material.Roughness), 0.05, 0.0f, 1.0f);
         ImGui::DragFloat("Metallic", &(material.Metallic), 0.05, 0.0f, 1.0f);

         ImGui::Separator();
         ImGui::PopID();
      }

      ImGui::End();

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport");

      m_ViewportWidth  = ImGui::GetContentRegionAvail().x;
      m_ViewportHeight = ImGui::GetContentRegionAvail().y;

      if (auto finalImage = m_Renderer.GetFinalImage())
      {
         ImGui::Image(  finalImage->GetDescriptorSet(),
                        { (float)finalImage->GetWidth(), (float)finalImage->GetHeight() },
                        ImVec2(0, 1), ImVec2(1, 0)); // Flip UVs
      }

      ImGui::End();
      ImGui::PopStyleVar();

      Render();
   }

   void Render()
   {
      Timer timer;

      // Render
      {
         // Resize if needed
         {
            m_Renderer.Resize(m_ViewportWidth, m_ViewportHeight);
            m_Camera.Resize(m_ViewportWidth, m_ViewportHeight);
         }

         m_Renderer.Render(m_Scene, m_Camera);
      }

      m_LastRenderTime = timer.ElapsedMillis();
   }

private:
   Renderer m_Renderer;
   Camera m_Camera;
   Scene m_Scene;

   uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

   float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
   Walnut::ApplicationSpecification spec;
   spec.Name = "CPU RayTracing";

   Walnut::Application* app = new Walnut::Application(spec);
   app->PushLayer<ExampleLayer>();
   app->SetMenubarCallback([app]()
   {
      if (ImGui::BeginMenu("File"))
      {
         if (ImGui::MenuItem("Exit"))
         {
            app->Close();
         }
         ImGui::EndMenu();
      }
   });
   return app;
}