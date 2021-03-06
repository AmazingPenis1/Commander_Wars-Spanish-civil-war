#ifndef UNITSPRITEMANAGER_H
#define UNITSPRITEMANAGER_H


#include "game/GameEnums.h"
#include "resource_management/RessourceManagement.h"

class UnitSpriteManager : public QObject, public RessourceManagement<UnitSpriteManager>
{
    Q_OBJECT
public:
    /**
     * @brief getUnitType
     * @param i
     * @return
     */
    qint32 getUnitType(qint32 i);
    /**
     * @brief UnitSpriteManager::getUnitType
     * @param id
     * @return
     */
    qint32 getUnitType(QString id);
    /**
     * @brief getUnitsSorted
     * @return
     */
    QStringList getUnitsSorted();
    /**
     * @brief getUnitTypeText
     * @param type
     * @return
     */
    static QString getUnitTypeText(qint32 type);
protected:
    friend RessourceManagement<UnitSpriteManager>;
    UnitSpriteManager();
private:
    virtual ~UnitSpriteManager() = default;
};


#endif // UNITSPRITEMANAGER_H
