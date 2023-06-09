#pragma once

#include "UUID.h"
#include "Scene.h"
#include "Components.h"

#include "Ent/raw.githubusercontent.com_skypjack_entt_master_single_include_entt_entt.hpp"

class Entity
{
public:
   Entity();
   Entity(entt::entity entity, Scene* scene);
   Entity(const Entity& other) = default;
   ~Entity();

   operator entt::entity() const { return m_EntityHandle; }
   operator uint32_t() const { return (uint32_t)m_EntityHandle; } // ennt internal id
   operator bool() const { return m_EntityHandle != entt::null; }
   bool operator == (const Entity& other) const;
   bool operator != (const Entity& other) const;

   template<typename T, typename... Args>
   T& AddComponent(Args&&... args)
   {
      //assert(not HasComponent<T>());
      return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
   }

   template<typename T>
   bool RemoveComponent()
   {
      //assert(HasComponent<T>());
      return m_Scene->m_Registry.remove<T>(m_EntityHandle);
   }


   template<typename T>
   T& GetComponent()
   {
      //assert(HasComponent<T>());
      return m_Scene->m_Registry.get<T>(m_EntityHandle);
   }

   template<typename T>
   bool HasComponent()
   {
      return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
   }

   UUID GetUUID() { return GetComponent<IDComponent>().m_UUID; }

private:
   entt::entity m_EntityHandle = entt::null;
   Scene* m_Scene = nullptr; // Pointer to the owner of this entity
};

