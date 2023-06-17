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
