#ifndef HUMANPLAYERINPUTMENU_H
#define HUMANPLAYERINPUTMENU_H

#include <QObject>

#include <QVector>

#include <QStringList>

#include "oxygine-framework.h"

#include "oxygine/KeyEvent.h"

class HumanPlayerInputMenu;
typedef oxygine::intrusive_ptr<HumanPlayerInputMenu> spHumanPlayerInputMenu;

class HumanPlayerInputMenu : public QObject, public oxygine::Actor
{
    Q_OBJECT
public:
    explicit HumanPlayerInputMenu(QStringList texts, QStringList actionIDs, QVector<oxygine::spActor> icons,
                                  QVector<qint32> costList = QVector<qint32>(), QVector<bool> enabledList = QVector<bool>());
    /**
     * @brief setMenuPosition changes the position of this menu
     * @param x position in pixel
     * @param y position in pixel
     */
    void setMenuPosition(qint32 x, qint32 y);
signals:
    void sigItemSelected(QString actionID, qint32 cost);
    void sigCanceled(qint32 x, qint32 y);
public slots:
    void leftClick(qint32 x, qint32 y);
    void keyInput(oxygine::KeyEvent event);
    void moveMouseToItem(qint32 x, qint32 y);
private:
    oxygine::spSprite m_Cursor;
    qint32 startY{0};
    qint32 itemHeigth{0};
    qint32 itemWidth{0};
    qint32 currentAction{0};
    QStringList m_ActionIDs;
    QVector<qint32> m_CostList;
    QVector<bool> m_EnabledList;
    qint32 createTopSprite(qint32 x, qint32 width);
    qint32 createBottomSprite(qint32 x, qint32 y, qint32 width);

    bool m_Focused{true};
};

#endif // HUMANPLAYERINPUTMENU_H
