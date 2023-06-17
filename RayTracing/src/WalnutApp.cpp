#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "glm/gtc/type_ptr.hpp"

// Misc Walnut
#include "Walnut/Timer.h"

// Raytracing specific
#include "Renderer.h"
#include "Camera.h"

// ECS
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:

   ExampleLayer()
      : m_Camera(45.0f, 0.1f, 100.0f)
   {
#pragma region CreateMaterials
      {
         Material purpleMat;
         purpleMat.m_Albedo = { 1.0f, 0.0f, 1.0f };
         purpleMat.m_Roughness = 0.4f;
         purpleMat.m_Metallic = 0.0f;
         purpleMat.m_EmissionPower = 0.0f;
         m_Materials.push_back(purpleMat);
      }

      {
         Material greenMat;
         greenMat.m_Albedo = { 0.0f, 1.0f, 0.0f };
         greenMat.m_Roughness = 0.1f;
         greenMat.m_Metallic = 1.0f;
         greenMat.m_EmissionPower = 0.0f;
         m_Materials.push_back(greenMat);
      }

      {
         Material sunMat;
         sunMat.m_Albedo = { 0.8f, 0.5f, 0.2f };
         sunMat.m_Roughness = 0.1f;
         sunMat.m_Metallic = 1.0f;
         sunMat.m_EmissionPower = 30.0f;
         m_Materials.push_back(sunMat);
      }
#pragma endregion

#pragma region CreateMeshes
      {
         Mesh mesh;
         mesh.m_Vertices = new Vertex[3];
         mesh.m_Vertices[0].m_Position = glm::vec3(1.0f, 1.0f, 0.0f);    //Top Right
         mesh.m_Vertices[1].m_Position = glm::vec3(-1.0f, -1.0f, 0.0f);  //Bottom Left
         mesh.m_Vertices[2].m_Position = glm::vec3(1.0f, -1.0f, 0.0f);   //Bottom Right

         glm::vec3 normal = glm::cross(mesh.m_Vertices[0].m_Position - mesh.m_Vertices[1].m_Position, mesh.m_Vertices[0].m_Position - mesh.m_Vertices[2].m_Position);
         mesh.m_Vertices[0].m_Normal = mesh.m_Vertices[1].m_Normal = mesh.m_Vertices[2].m_Normal = glm::normalize(normal); // All vertices got the same normal obv

         m_Meshes.push_back(mesh);
      }
#pragma endregion

      Entity sphere = m_Scene.CreateEntity("Sphere");
      Entity floor = m_Scene.CreateEntity("Floor");
      Entity sun  = m_Scene.CreateEntity("Sun");
      Entity triangle = m_Scene.CreateEntity("Triangle");

      // Create Sceneobjects
      {
         // Sphere
         sphere.AddComponent<SphereComponent>(glm::vec3(2.5f, 0.0f, 0.5f), 1.0f);
         sphere.AddComponent<MaterialComponent>(&m_Materials[0]);
         
         // Floor (big Sphere moved down)
         floor.AddComponent<SphereComponent>(glm::vec3(0.0f, -101.f, 0.0f), 100.0f);
         floor.AddComponent<MaterialComponent>(&m_Materials[1]);
         
         // Sun (Sphere with emissive)
         sun.AddComponent<SphereComponent>(glm::vec3(25.0f, 4.0f, -25.0f), 20.0f);
         sun.AddComponent<MaterialComponent>(&m_Materials[2]);
         
         // Triangle
         triangle.AddComponent<MeshComponent>(&m_Meshes[0]);
         triangle.AddComponent<MaterialComponent>(&m_Materials[0]);
      }
   };

   virtual void OnUpdate(float ts) override
   {
      if (m_Camera.Update(ts))
      {
         m_Renderer.ResetFrameIndex();
      }
   }

   virtual void OnUIRender() override
   {
      ImGui::Begin("Settings");
      ImGui::Text("Last render: %.3fms", m_LastRenderTime);

      ImGui::Checkbox("Acuumulate", &m_Renderer.GetSettings().Accumulate);
      if (ImGui::Button("Reset"))
      {
         m_Renderer.ResetFrameIndex();
      }

      ImGui::End();

      ImGui::Begin("Scene");
      //if (ImGui::TreeNode("Spheres"))
      //{
      //   for (size_t i = 0; i < m_Scene.m_Spheres.size(); i++)
      //   {
      //      ImGui::PushID(i);
      //
      //      Sphere& sphere = m_Scene.m_Spheres[i];
      //      ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
      //      ImGui::DragFloat("Radius", &(sphere.Radius), 0.1f);
      //      ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.m_Materials.size() - 1);
      //
      //      ImGui::Separator();
      //      ImGui::PopID();
      //   }
      //   ImGui::TreePop();
      //}
      //
      //if (ImGui::TreeNode("Materials"))
      //{
      //   for (size_t i = 0; i < m_Scene.m_Materials.size(); i++)
      //   {
      //      ImGui::PushID(i);
      //
      //      Material& material = m_Scene.m_Materials[i];
      //      ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo), 0.1f);
      //      ImGui::DragFloat("Roughness", &(material.Roughness), 0.05, 0.0f, 1.0f);
      //      ImGui::DragFloat("Metallic", &(material.Metallic), 0.05, 0.0f, 1.0f);
      //
      //      ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
      //      ImGui::DragFloat("Emission Power", &(material.EmissionPower), 0.05, 0.0f, FLT_MAX);
      //
      //      ImGui::Separator();
      //      ImGui::PopID();
      //   }
      //
      //   ImGui::TreePop();
      //}

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

   // Should be in some sort of assetmanager class
   std::vector<Material> m_Materials;
   std::vector<Mesh> m_Meshes;

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