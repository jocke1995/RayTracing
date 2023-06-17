#pragma once

#include "UUID.h"

#include "Ent/raw.githubusercontent.com_skypjack_entt_master_single_include_entt_entt.hpp"

#include "glm/glm.hpp"
#include <vector>
#include <string>

class Entity;

class Scene
{
public:
   Scene();
   ~Scene();

   Entity CreateEntity(std::string tag = "");
   Entity CreateEntityWithUUID(UUID uuid, std::string tag = "");

   Entity GetEntityByUUID(UUID uuid);

   template<typename... Components>
   auto GetAllEntitiesWith() const
   {
      return m_Registry.view<Components...>();
   }

private:
   friend class Entity;

   entt::registry m_Registry;

   std::unordered_map<UUID, entt::entity> m_EntityMap;
};
