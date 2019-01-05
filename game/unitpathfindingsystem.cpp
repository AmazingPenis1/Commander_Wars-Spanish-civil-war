#include "game/unitpathfindingsystem.h"

#include "resource_management/movementtablemanager.h"

#include "game/gamemap.h"

#include "game/player.h"

UnitPathFindingSystem::UnitPathFindingSystem(spUnit pUnit)
    : PathFindingSystem(pUnit->getX(), pUnit->getY()),
      m_pUnit(pUnit)
{
}

qint32 UnitPathFindingSystem::getRemainingCost(qint32 x, qint32 y, qint32 currentCost)
{
    GameMap* pMap = GameMap::getInstance();
    if (pMap->onMap(x, y))
    {
        return m_pUnit->getMovementPoints() - currentCost;
    }
    else
    {
        return -1;
    }
}

bool UnitPathFindingSystem::finished(qint32, qint32)
{
    return false;
}

qint32 UnitPathFindingSystem::getCosts(qint32 x, qint32 y)
{
    MovementTableManager* pMovementTableManager = MovementTableManager::getInstance();
    GameMap* pMap = GameMap::getInstance();
    if (pMap->onMap(x, y))
    {
        Unit* pUnit = pMap->getTerrain(x, y)->getUnit();
        // check for an enemy on the field
        if (pUnit != nullptr)
        {
            if (m_pUnit->getOwner()->checkAlliance(pUnit->getOwner()) == Player::Alliance::Enemy)
            {
                return -1;
            }
        }
        return pMovementTableManager->getMovementPoints(m_pUnit->getMovementType(), pMap->getTerrain(x, y)->getID());
    }
    else
    {
        return -1;
    }
}