#include "EntityManager.h"

EntityManager::EntityManager() = default;

void EntityManager::update()
{
    // Add pending entities to main list and tagged map
    for (auto& entity : m_entitiesToAdd)
    {
        m_entities.push_back(entity);
        m_entityMap[entity->tag()].push_back(entity);
    }
    m_entitiesToAdd.clear();

    // Skip dead entity removal for now (Step 5)
    // removeDeadEntities(m_entities);
    // for (auto& [tag, entityVec] : m_entityMap) {
    //     removeDeadEntities(entityVec);
    // }
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
    // Temporary empty implementation (will complete in Step 5)
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    // Use 'new' to access the private constructor (allowed because EntityManager is a friend)
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return m_entityMap[tag];
}