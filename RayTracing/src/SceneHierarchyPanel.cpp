#include "SceneHierarchyPanel.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

#include "imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

SceneHierarchyPanel::SceneHierarchyPanel(Scene* scene)
{
   SetScene(scene);
}

SceneHierarchyPanel::~SceneHierarchyPanel()
{
}

void SceneHierarchyPanel::SetScene(Scene* scene)
{
   m_Scene = scene;
   m_SelectedEntity = {};
}

void SceneHierarchyPanel::RenderSceneHierarchy()
{
   ImGui::Begin("Scene Hierarchy");


   m_Scene->m_Registry.each([&](auto entityID)
   {
         Entity entity{ entityID, m_Scene };
         DrawEntityNode(entity);
   });

   ImGui::End();

   ImGui::Begin("Properties");

   if (m_SelectedEntity)
   {
      DrawComponents(m_SelectedEntity);
   }
   ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
   auto& tc = entity.GetComponent<TagComponent>();

   ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
   if (m_SelectedEntity == entity)
   {
      flags |= ImGuiTreeNodeFlags_Selected;
   }

   bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tc.m_Tag.c_str());
   
   if(ImGui::IsItemClicked())
   {
      m_SelectedEntity = entity;
   
   }
   
   if (opened)
   {
      ImGui::TreePop();
   }
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
   if (entity.HasComponent<TagComponent>())
   {
      TagComponent& tc = entity.GetComponent<TagComponent>();

      //if (ImGui::TreeNode("TagComponent"))
      {
         char buffer[256];
         memset(buffer, 0, sizeof(buffer));
         strcpy_s(buffer, sizeof(buffer), tc.m_Tag.c_str());

         if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
         {
            tc.m_Tag = std::string(buffer);
         }

         //ImGui::TreePop();
      }
   }

   if (entity.HasComponent<SphereComponent>())
   {
      ImGui::Separator();
      SphereComponent& sc = entity.GetComponent<SphereComponent>();

      ImGui::DragFloat3("Position", glm::value_ptr(sc.m_Position), 0.1f);
      ImGui::DragFloat("Radius", &(sc.m_Radius), 0.1f);
   }

   if (entity.HasComponent<MaterialComponent>())
   {
      ImGui::Separator();
      MaterialComponent& mc = entity.GetComponent<MaterialComponent>();

      Material& material = *mc.m_Material;
      ImGui::ColorEdit3("Albedo", glm::value_ptr(material.m_Albedo), 0.1f);
      ImGui::DragFloat("Roughness", &(material.m_Roughness), 0.05, 0.0f, 1.0f);
      ImGui::DragFloat("Metallic", &(material.m_Metallic), 0.05, 0.0f, 1.0f);
      ImGui::DragFloat("Emission Power", &(material.m_EmissionPower), 0.05, 0.0f, FLT_MAX);
   }
   //ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.m_Materials.size() - 1);
}
