#ifndef GAMEPLAYANDKEYS_H
#define GAMEPLAYANDKEYS_H

#include "qobject.h"

#include "oxygine-framework.h"

#include "objects/panel.h"

class GameplayAndKeys;
typedef oxygine::intrusive_ptr<GameplayAndKeys> spGameplayAndKeys;

class GameplayAndKeys : public QObject, public oxygine::Actor
{
    Q_OBJECT
public:
    GameplayAndKeys(qint32 heigth);

private:
    spPanel m_pOptions;
};

#endif // GAMEPLAYANDKEYS_H
