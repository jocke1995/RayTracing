#pragma once

#include "Core.h"
#include "Scene/Entity.h"
class Scene;

class SceneHierarchyPanel
{
public:
   SceneHierarchyPanel(Scene* scene);
   ~SceneHierarchyPanel();

   void SetScene(Scene* scene);

   void RenderSceneHierarchy();
private:
   void DrawEntityNode(Entity entity);
   void DrawComponents(Entity entity);

   Scene* m_Scene = nullptr;
   Entity m_SelectedEntity = {};
};