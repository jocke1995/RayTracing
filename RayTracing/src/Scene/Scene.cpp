#include "Scene.h"
#include "Entity.h"
#include "Components.h"

Scene::Scene()
{
}

Scene::~Scene()
{

}

Entity Scene::CreateEntity(std::string tag)
{
   return CreateEntityWithUUID(UUID(), tag);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, std::string tag)
{
   Entity entity = { m_Registry.create(), this };

   entity.AddComponent<IDComponent>(uuid);
   TagComponent& tagComponent = entity.AddComponent<TagComponent>(tag);
   if (tag.empty() == true)
   {
      tagComponent.m_Tag = "Unnamed Entity";
   }

   m_EntityMap[uuid] = entity; // Implicit operator to entt::entity

   // Add components that entities get assigned automaticaly. Like a tag/transform
   return entity;
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
   if (m_EntityMap.find(uuid) != m_EntityMap.end())
   {
      return { m_EntityMap.at(uuid), this };
   }

   return { entt::null, nullptr};
}
