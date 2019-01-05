#include "game/player.h"

#include "coreengine/mainapp.h"
#include "gameinput/basegameinputif.h"

Player::Player(quint32 id)
    : playerID(id)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function = "loadDefaultPlayerColor";
    QJSValueList args;
    QJSValue objArg = pApp->getInterpreter()->newQObject(this);
    args << objArg;
    pApp->getInterpreter()->doFunction("PLAYER", function, args);
}

Player::~Player()
{
    if (m_pBaseGameInput != nullptr)
    {
        delete m_pBaseGameInput;
    }
}

QColor Player::getColor() const
{
    return m_Color;
}

void Player::setColor(QColor color)
{
    m_Color = color;
}

quint32 Player::getPlayerID() const
{
    return playerID;
}

void Player::setPlayerID(const quint32 &value)
{
    playerID = value;
}

QString Player::getArmy()
{
    // todo return ko army
    Mainapp* pApp = Mainapp::getInstance();
    QJSValueList args;
    QJSValue objArg = pApp->getInterpreter()->newQObject(this);
    args << objArg;
    QJSValue ret = pApp->getInterpreter()->doFunction("PLAYER", "getDefaultArmy", args);
    if (ret.isString())
    {
        return ret.toString();
    }
    else
    {
        return "OS";
    }
}

Player::Alliance Player::checkAlliance(Player* pPlayer)
{
    if (pPlayer == this)
    {
        return Alliance::Friend;
    }
    else
    {
        // todo implement real check for alliance
        return Alliance::Enemy;
    }
}

void Player::setBaseGameInput(BaseGameInputIF *pBaseGameInput)
{
    m_pBaseGameInput = pBaseGameInput;
    m_pBaseGameInput->setPlayer(this);
}

void Player::serialize(QDataStream& pStream)
{
    pStream << getVersion();
    quint32 color = m_Color.rgb();
    pStream << color;
}
void Player::deserialize(QDataStream& pStream)
{
    qint32 version = 0;
    pStream >> version;
    quint32 color;
    pStream >> color;
    m_Color.fromRgb(color);
}