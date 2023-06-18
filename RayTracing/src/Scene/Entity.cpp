#include "Entity.h"

Entity::Entity()
{
}

Entity::Entity(entt::entity entity, Scene* scene)
   : m_EntityHandle(entity), m_Scene(scene)
{
}

Entity::~Entity()
{
}

bool Entity::operator==(const Entity& other) const
{
    return (m_EntityHandle == other.m_EntityHandle) && (m_Scene == other.m_Scene);
}

bool Entity::operator!=(const Entity& other) const
{
   return not operator==(other);
}
