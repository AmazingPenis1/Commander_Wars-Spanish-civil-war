#include "multiplayermenu.h"

#include "qcryptographichash.h"

#include "qdir.h"
#include "qdiriterator.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"
#include "coreengine/settings.h"
#include "coreengine/filesupport.h"

#include "multiplayer/lobbymenu.h"
#include "menue/gamemenue.h"

#include "network/tcpclient.h"
#include "network/tcpserver.h"

#include "game/gamemap.h"
#include "game/player.h"
#include "game/co.h"

#include "objects/dialogconnecting.h"
#include "objects/filedialog.h"
#include "objects/dialogmessagebox.h"

#include "ingamescriptsupport/genericbox.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/fontmanager.h"

#include "multiplayer/networkcommands.h"


Multiplayermenu::Multiplayermenu(QString adress, bool host)
    : MapSelectionMapsMenue(Settings::getHeight() - 380),
      m_Host(host),
      m_local(true)
{
    init();
    if (!host)
    {
        m_NetworkInterface = new TCPClient();
        m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
        initClientAndWaitForConnection();
        emit m_NetworkInterface->sig_connect(adress, Settings::getGamePort());
    }
    else
    {
        m_pHostAdresse = ObjectManager::createButton("Show Adresses");
        addChild(m_pHostAdresse);
        m_pHostAdresse->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
        {
            emit sigShowIPs();
        });
        m_pHostAdresse->setPosition(m_pButtonStart->getX() - m_pHostAdresse->getWidth() - 10,
                                    Settings::getHeight() - m_pHostAdresse->getHeight() - 10);
        m_pHostAdresse->setVisible(false);
        connect(this, &Multiplayermenu::sigShowIPs, this, &Multiplayermenu::showIPs, Qt::QueuedConnection);
    }
}

Multiplayermenu::Multiplayermenu(spNetworkInterface pNetworkInterface, bool host)
    : MapSelectionMapsMenue(Settings::getHeight() - 380),
      m_Host(host),
      m_local(false)
{
    m_NetworkInterface = pNetworkInterface.get();
    init();
    if (!host)
    {
        initClientAndWaitForConnection();
    }
    else
    {
        dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Ready"));
        m_pPlayerSelection->setIsServerGame(true);
    }
}

void Multiplayermenu::initClientAndWaitForConnection()
{
    createChat();
    hideRuleSelection();
    hideMapSelection();
    m_MapSelectionStep = MapSelectionStep::selectPlayer;
    // change the name of the start button
    dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Ready"));
    // quit on host connection lost
    connect(m_NetworkInterface.get(), &NetworkInterface::sigDisconnected, this, &Multiplayermenu::disconnected, Qt::QueuedConnection);
    connect(m_NetworkInterface.get(), &NetworkInterface::recieveData, this, &Multiplayermenu::recieveData, Qt::QueuedConnection);
    connect(m_pPlayerSelection.get(), &PlayerSelection::sigDisconnect, this, &Multiplayermenu::slotButtonBack, Qt::QueuedConnection);
    // wait 10 minutes till timeout
    spDialogConnecting pDialogConnecting = new DialogConnecting(tr("Connecting"), 1000 * 60 * 5);
    addChild(pDialogConnecting);
    connect(pDialogConnecting.get(), &DialogConnecting::sigCancel, this, &Multiplayermenu::slotButtonBack, Qt::QueuedConnection);
    connect(this, &Multiplayermenu::sigConnected, pDialogConnecting.get(), &DialogConnecting::connected, Qt::QueuedConnection);
}

void Multiplayermenu::init()
{
    Mainapp* pApp = Mainapp::getInstance();
    this->moveToThread(pApp->getWorkerthread());
    Console::print("Entering Multiplayer Menue", Console::eDEBUG);
    m_pButtonLoadSavegame = ObjectManager::createButton(tr("Load Savegame"));
    m_pButtonLoadSavegame->setPosition(Settings::getWidth() - m_pButtonLoadSavegame->getWidth() - m_pButtonNext->getWidth() - 20, Settings::getHeight() - 10 - m_pButtonLoadSavegame->getHeight());
    m_pButtonLoadSavegame->attachTo(this);
    m_pButtonLoadSavegame->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigLoadSaveGame();
    });
    connect(this, &Multiplayermenu::sigLoadSaveGame, this, &Multiplayermenu::showLoadSaveGameDialog, Qt::QueuedConnection);
    connect(&m_GameStartTimer, &QTimer::timeout, this, &Multiplayermenu::countdown, Qt::QueuedConnection);
}

void Multiplayermenu::showIPs()
{
    spGenericBox pGenericBox = new GenericBox();
    QStringList items = NetworkInterface::getIPAdresses();
    QSize size(Settings::getWidth() - 40, Settings::getHeight() - 80);
    spPanel pPanel = new Panel(true, size, size);
    pPanel->setPosition(20, 20);
    oxygine::TextStyle style = FontManager::getMainFont24();
    style.color = FontManager::getFontColor();
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = true;
    oxygine::spTextField info = new oxygine::TextField();
    info->setStyle(style);
    info->setHtmlText((tr("Please use one of the following IP-Adresses to connect to this Host. Not all IP-Adresses") +
                      tr(" may work for each client depending on the network settings. Please use cmd and the ping command to verify if an IP-Adress may work")));
    info->setSize(Settings::getWidth() - 80, 500);
    info->setPosition(10, 10);
    pPanel->addItem(info);
    qint32 starty = 10 + info->getTextRect().getHeight();
    for (qint32 i = 0; i < items.size(); i++)
    {
        oxygine::spTextField text = new oxygine::TextField();
        text->setStyle(style);
        text->setHtmlText((tr("Host Adress: ") + items[i]));
        text->setPosition(10, starty + i * 40);
        pPanel->addItem(text);
    }
    pPanel->setContentHeigth(items.size() * 40 + 40);
    pGenericBox->addItem(pPanel);
    addChild(pGenericBox);
}

void Multiplayermenu::showLoadSaveGameDialog()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    // dummy impl for loading
    QVector<QString> wildcards;
    wildcards.append("*.msav");
    QString path = QCoreApplication::applicationDirPath() + "/savegames";
    spFileDialog saveDialog = new FileDialog(path, wildcards);
    this->addChild(saveDialog);
    connect(saveDialog.get(), &FileDialog::sigFileSelected, this, &Multiplayermenu::loadSaveGame, Qt::QueuedConnection);
    pApp->continueThread();
}

void Multiplayermenu::loadSaveGame(QString filename)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    if (filename.endsWith(".msav"))
    {
        QFile file(filename);
        QFileInfo info(filename);
        if (file.exists())
        {
            m_pMapSelectionView->loadMap(info);
            hideMapSelection();
            saveGame = true;
            m_pPlayerSelection->setSaveGame(saveGame);
            m_MapSelectionStep = MapSelectionStep::selectRules;
            slotButtonNext();
        }
    }
    pApp->continueThread();
}

void Multiplayermenu::hideMapSelection()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pButtonLoadSavegame->setVisible(false);
    MapSelectionMapsMenue::hideMapSelection();
    pApp->continueThread();
}

void Multiplayermenu::showMapSelection()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pButtonLoadSavegame->setVisible(true);
    MapSelectionMapsMenue::showMapSelection();
    pApp->continueThread();
}

void Multiplayermenu::playerJoined(quint64 socketID)
{
    if (m_NetworkInterface->getIsServer() && Mainapp::getSlave())
    {
        if (m_slaveGameReady)
        {
            sendSlaveReady();
        }
    }
    else if (m_NetworkInterface->getIsServer() && m_local)
    {
        if (m_pPlayerSelection->hasOpenPlayer())
        {
            acceptNewConnection(socketID);
        }
        else
        {
            // reject connection by disconnecting
            emit m_NetworkInterface.get()->sigDisconnectClient(socketID);
        }
    }
}

void Multiplayermenu::acceptNewConnection(quint64 socketID)
{
    QCryptographicHash myHash(QCryptographicHash::Sha3_512);
    QString file = m_pMapSelectionView->getCurrentFile().filePath();
    QString fileName = m_pMapSelectionView->getCurrentFile().fileName();
    QString scriptFile = m_pMapSelectionView->getCurrentMap()->getGameScript()->getScriptFile();
    QFile mapFile(file);
    mapFile.open(QIODevice::ReadOnly);
    myHash.addData(&mapFile);
    mapFile.close();
    QByteArray hash = myHash.result();
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << NetworkCommands::MAPINFO;
    stream << Mainapp::getGameVersion();
    QStringList mods = Settings::getMods();
    QStringList versions = Settings::getActiveModVersions();
    stream << static_cast<qint32>(mods.size());
    for (qint32 i = 0; i < mods.size(); i++)
    {
        stream << mods[i];
        stream << versions[i];
    }
    Filesupport::writeByteArray(stream, Filesupport::getRuntimeHash());
    stream << saveGame;
    if (saveGame)
    {
        m_pMapSelectionView->getCurrentMap()->serializeObject(stream);
    }
    else
    {
        stream << fileName;
        stream << hash;
        stream << scriptFile;
        if (QFile::exists(scriptFile))
        {
            // create hash for script file
            QFile scriptData(scriptFile);
            QCryptographicHash myScriptHash(QCryptographicHash::Sha3_512);
            scriptData.open(QIODevice::ReadOnly);
            myScriptHash.addData(&scriptData);
            scriptData.close();
            QByteArray scriptHash = myScriptHash.result();
            stream << scriptHash;
        }
    }
    // send map data to client
    m_NetworkInterface->sig_sendData(socketID, data, NetworkInterface::NetworkSerives::Multiplayer, false);
}

void Multiplayermenu::recieveData(quint64 socketID, QByteArray data, NetworkInterface::NetworkSerives service)
{
    // data for us?
    if (service == NetworkInterface::NetworkSerives::Multiplayer)
    {
        if (!m_pPlayerSelection->hasNetworkInterface())
        {
            m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
        }
        QDataStream stream(&data, QIODevice::ReadOnly);
        QString messageType;
        stream >> messageType;
        Console::print("Local Network Command received: " + messageType + " for socket " + QString::number(socketID), Console::eDEBUG);
        if (messageType == NetworkCommands::MAPINFO)
        {
            clientMapInfo(stream, socketID);
        }
        else if (messageType == NetworkCommands::REQUESTRULE)
        {
            requestRule(socketID);
        }
        else if (messageType == NetworkCommands::SENDINITUPDATE)
        {
            sendInitUpdate(stream, socketID);
        }
        else if (messageType == NetworkCommands::REQUESTMAP)
        {
            requestMap(socketID);
        }
        else if (messageType == NetworkCommands::MAPDATA)
        {
            recieveMap(stream, socketID);
        }
        else if (messageType == NetworkCommands::INITGAME)
        {
            // initializes the game on the client
            if (!m_NetworkInterface->getIsServer() ||
                !m_local)
            {
                initClientGame(socketID, stream);
                addRef();
                oxygine::Actor::detach();
                deleteLater();
            }
        }
        else if (messageType == NetworkCommands::STARTSERVERGAME)
        {
            startCountdown();
        }
    }
    else if (service == NetworkInterface::NetworkSerives::ServerHosting)
    {
        QDataStream stream(&data, QIODevice::ReadOnly);
        QString messageType;
        stream >> messageType;
        Console::print("Recieving data from Master. Command received: " + messageType + " for socket " + QString::number(socketID), Console::eDEBUG);
        if (messageType == NetworkCommands::LAUNCHGAMEONSERVER)
        {
            launchGameOnServer(stream);
        }
        else if (messageType == NetworkCommands::PLAYERJOINEDGAMEONSERVER)
        {
            if (m_pPlayerSelection->hasOpenPlayer())
            {
                quint64 socketId;
                stream >> socketId;
                acceptNewConnection(socketID);
            }
            else
            {
                // todo deny access
            }
        }
        else if (messageType == NetworkCommands::PLAYERDISCONNECTEDGAMEONSERVER)
        {
            quint64 socketId;
            stream >> socketId;
            m_pPlayerSelection->disconnected(socketID);
        }
    }
}

void Multiplayermenu::requestRule(quint64 socketID)
{
    // a client requested the current map rules set by the server
    if (m_NetworkInterface->getIsServer())
    {
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << NetworkCommands::SENDINITUPDATE;
        m_pMapSelectionView->getCurrentMap()->getGameRules()->serializeObject(sendStream);
        if (m_pMapSelectionView->getCurrentMap()->getCampaign() != nullptr)
        {
            sendStream << true;
            m_pMapSelectionView->getCurrentMap()->getCampaign()->serializeObject(sendStream);
        }
        else
        {
            sendStream << false;
        }
        for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
        {
            GameEnums::AiTypes aiType = m_pPlayerSelection->getPlayerAiType(i);
            if (aiType == GameEnums::AiTypes_Human)
            {
                sendStream << Settings::getUsername();
                sendStream << static_cast<qint32>(GameEnums::AiTypes_ProxyAi);
            }
            else
            {
                sendStream << m_pPlayerSelection->getPlayerAiName(i);
                if (m_pPlayerSelection->isOpenPlayer(i))
                {
                    sendStream << static_cast<qint32>(GameEnums::AiTypes_Open);
                }
                else
                {
                    sendStream << static_cast<qint32>(GameEnums::AiTypes_ProxyAi);
                }
            }
            m_pMapSelectionView->getCurrentMap()->getPlayer(i)->serializeObject(sendStream);
        }
        m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::sendInitUpdate(QDataStream & stream, quint64 socketID)
{
    // the client recieved the initial map data read it and make it visible
    if (!m_NetworkInterface->getIsServer() || !m_local)
    {
        m_pMapSelectionView->getCurrentMap()->getGameRules()->deserializeObject(stream);
        bool campaign = false;
        stream >> campaign;
        if (campaign)
        {
            m_pMapSelectionView->getCurrentMap()->setCampaign(new Campaign());
            m_pMapSelectionView->getCurrentMap()->getCampaign()->deserializeObject(stream);
        }
        for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
        {
            QString name;
            qint32 aiType;
            stream >> name;
            stream >> aiType;
            m_pPlayerSelection->setPlayerAiName(i, name);
            m_pMapSelectionView->getCurrentMap()->getPlayer(i)->deserializeObject(stream);
            m_pMapSelectionView->getCurrentMap()->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(static_cast<GameEnums::AiTypes>(aiType)));
            m_pPlayerSelection->updatePlayerData(i);
        }
        m_pPlayerSelection->sendPlayerRequest(socketID, -1, GameEnums::AiTypes_Human);
        emit sigConnected();
        emit sigHostGameLaunched();
    }
}

void Multiplayermenu::clientMapInfo(QDataStream & stream, quint64 socketID)
{
    // we recieved map info from a server check if we have this map already
    if (!m_NetworkInterface->getIsServer() || !m_local)
    {
        QString version;
        stream >> version;
        qint32 size = 0;
        stream >> size;
        QStringList mods;
        QStringList versions;
        for (qint32 i = 0; i < size; i++)
        {
            QString mod;
            stream >> mod;
            mods.append(mod);
            QString version;
            stream >> version;
            versions.append(version);
        }
        QStringList myMods = Settings::getMods();
        QStringList myVersions = Settings::getActiveModVersions();
        bool sameMods = true;
        if (myMods.size() != mods.size())
        {
            sameMods = false;
        }
        else
        {
            // check mods in both directions
            for (qint32 i = 0; i < myMods.size(); i++)
            {
                if (!mods.contains(myMods[i]))
                {
                    sameMods = false;
                    break;
                }
                else
                {
                    for (qint32 i2 = 0; i2 < mods.size(); i2++)
                    {
                        if (mods[i2] == myMods[i])
                        {
                            if (versions[i2] != myVersions[i])
                            {
                                sameMods = false;
                                break;
                            }
                        }
                    }
                }
            }
            for (qint32 i = 0; i < mods.size(); i++)
            {
                if (!myMods.contains(mods[i]))
                {
                    sameMods = false;
                    break;
                }
            }
        }
        QByteArray hostRuntime = Filesupport::readByteArray(stream);
        if (hostRuntime != Filesupport::getRuntimeHash())
        {
            sameMods = false;
        }
        if (version == Mainapp::getGameVersion() && sameMods)
        {
            stream >> saveGame;
            if (saveGame)
            {
                m_pMapSelectionView->setCurrentMap(new GameMap(stream));
                loadMultiplayerMap();
                QByteArray sendData;
                QDataStream sendStream(&sendData, QIODevice::WriteOnly);
                sendStream << NetworkCommands::REQUESTRULE;
                m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
            }
            else
            {
                QString fileName;
                stream >> fileName;
                QByteArray hash;
                stream >> hash;
                QString scriptFile;
                stream >> scriptFile;
                QByteArray scriptHash;
                if (!scriptFile.isEmpty())
                {
                    stream >> scriptHash;
                }
                if (!fileName.startsWith(NetworkCommands::RANDOMMAPIDENTIFIER) &&
                    !fileName.startsWith(NetworkCommands::SERVERMAPIDENTIFIER) &&
                    existsMap(fileName, hash, scriptFile, scriptHash))
                {
                    QByteArray sendData;
                    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
                    sendStream << NetworkCommands::REQUESTRULE;
                    m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
                }
                else
                {
                    QByteArray sendData;
                    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
                    sendStream << NetworkCommands::REQUESTMAP;
                    m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
                }
            }
        }
        else
        {
            // quit game with wrong version
            Mainapp* pApp = Mainapp::getInstance();
            pApp->suspendThread();
            spDialogMessageBox pDialogMessageBox = new DialogMessageBox(tr("Host has a different game version or other mods loaded leaving the game again."));
            connect(pDialogMessageBox.get(), &DialogMessageBox::sigOk, this, &Multiplayermenu::slotButtonBack, Qt::QueuedConnection);
            addChild(pDialogMessageBox);
            pApp->continueThread();
        }
    }
}

void Multiplayermenu::requestMap(quint64 socketID)
{
    // someone requested the current map data from the server
    if (m_NetworkInterface->getIsServer())
    {
        QString file = m_pMapSelectionView->getCurrentFile().filePath().replace(QCoreApplication::applicationDirPath() + "/", "");
        QString scriptFile = m_pMapSelectionView->getCurrentMap()->getGameScript()->getScriptFile();
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << NetworkCommands::MAPDATA;
        sendStream << file;
        sendStream << scriptFile;
        QFile mapFile(m_pMapSelectionView->getCurrentFile().filePath());
        if (file.startsWith(NetworkCommands::RANDOMMAPIDENTIFIER) ||
            file.startsWith(NetworkCommands::SERVERMAPIDENTIFIER))
        {
            GameMap::getInstance()->serializeObject(sendStream);
        }
        else
        {
            mapFile.open(QIODevice::ReadOnly);
            QByteArray data = mapFile.readAll();
            mapFile.close();
            Filesupport::writeByteArray(sendStream, data);
        }
        if (!scriptFile.isEmpty())
        {
            QFile script(scriptFile);
            script.open(QIODevice::ReadOnly);
            QByteArray data = script.readAll();
            Filesupport::writeByteArray(sendStream, data);
            script.close();
        }
        m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::recieveMap(QDataStream & stream, quint64 socketID)
{
    if (!m_NetworkInterface->getIsServer())
    {
        QString mapFile;
        stream >> mapFile;
        QString scriptFile;
        stream >> scriptFile;
        GameMap* pNewMap = nullptr;
        if (mapFile.startsWith(NetworkCommands::RANDOMMAPIDENTIFIER) ||
            mapFile.startsWith(NetworkCommands::SERVERMAPIDENTIFIER))
        {
            pNewMap = new GameMap(stream);
        }
        else
        {
            QFile map(mapFile);
            QFile script(scriptFile);
            if (!map.exists() &&
                (scriptFile.isEmpty() ||
                 !script.exists()))
            {
                QByteArray mapData;
                mapData = Filesupport::readByteArray(stream);
                QFileInfo mapInfo(mapFile);
                QDir dir;
                QString fileDir = mapInfo.filePath().replace(mapInfo.fileName(), "");
                dir.mkdir(fileDir);
                map.open(QIODevice::WriteOnly);
                QDataStream mapFilestream(&map);
                Filesupport::writeBytes(mapFilestream, mapData);
                map.close();
                if (!scriptFile.isEmpty())
                {
                    QByteArray scriptData;
                    scriptData = Filesupport::readByteArray(stream);
                    QFileInfo scriptInfo(scriptFile);
                    fileDir = scriptInfo.filePath().replace(scriptInfo.fileName(), "");
                    dir.mkdir(fileDir);
                    script.open(QIODevice::WriteOnly);
                    QDataStream scriptFilestream(&script);
                    Filesupport::writeBytes(scriptFilestream, scriptData);
                    script.close();
                }
                pNewMap = new GameMap(mapFile, true, false);
            }
            else
            {
                pNewMap = createMapFromStream(mapFile, scriptFile, stream);
            }
        }
        m_pMapSelectionView->setCurrentMap(pNewMap);
        loadMultiplayerMap();
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << NetworkCommands::REQUESTRULE;
        m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::launchGameOnServer(QDataStream & stream)
{
    Console::print("Launching Game on Slave", Console::eDEBUG);
    hideMapSelection();
    QStringList mods;
    mods = Filesupport::readVectorList<QString, QList>(stream);
    GameMap* pMap = new GameMap(stream);
    m_pMapSelectionView->setCurrentMap(pMap);
    m_pMapSelectionView->setCurrentFile(NetworkCommands::SERVERMAPIDENTIFIER);
    m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
    m_pPlayerSelection->setIsServerGame(true);
    loadMultiplayerMap();
    createChat();
    m_pPlayerSelection->setPlayerAi(0, GameEnums::AiTypes::AiTypes_Open, "");
    sendSlaveReady();
    m_slaveGameReady = true;
}

void Multiplayermenu::sendSlaveReady()
{
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << NetworkCommands::GAMERUNNINGONSERVER;
    m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::ServerHosting, true);
}

void Multiplayermenu::slotCancelHostConnection()
{
    // todo cancel game hosting
}

void Multiplayermenu::slotHostGameLaunched()
{
    // we're hosting a game so we get the same rights as a local host
    m_NetworkInterface->setIsServer(true);
    createChat();
    MapSelectionMapsMenue::slotButtonNext();
}

GameMap* Multiplayermenu::createMapFromStream(QString mapFile, QString scriptFile, QDataStream &stream)
{
    GameMap* pNewMap = nullptr;
    mapFile = getNewFileName(mapFile);
    QFile map(mapFile);
    QFileInfo mapInfo(mapFile);
    QDir dir;
    QString fileDir = mapInfo.filePath().replace(mapInfo.fileName(), "");
    dir.mkdir(fileDir);
    map.open(QIODevice::WriteOnly);
    QDataStream mapFilestream(&map);
    QByteArray mapData;
    mapData = Filesupport::readByteArray(stream);
    Filesupport::writeBytes(mapFilestream, mapData);
    map.close();
    pNewMap = new GameMap(mapFile, true, false);
    if (!scriptFile.isEmpty())
    {
        QFile script(scriptFile);
        scriptFile = getNewFileName(scriptFile);
        QByteArray scriptData;
        scriptData = Filesupport::readByteArray(stream);
        QFileInfo scriptInfo(scriptFile);
        fileDir = scriptInfo.filePath().replace(scriptInfo.fileName(), "");
        dir.mkdir(fileDir);
        script.open(QIODevice::WriteOnly);
        QDataStream scriptFilestream(&script);
        Filesupport::writeBytes(scriptFilestream, scriptData);
        script.close();
        scriptFile = scriptFile.replace(QCoreApplication::applicationDirPath() + "/", "");
        scriptFile = scriptFile.replace(QCoreApplication::applicationDirPath(), "");
        // save new script file
        pNewMap->getGameScript()->setScriptFile(scriptFile);
        map.open(QIODevice::WriteOnly);
        QDataStream stream(&map);
        pNewMap->serializeObject(stream);
        map.close();
    }
    return pNewMap;
}

QString Multiplayermenu::getNewFileName(QString filename)
{
    bool found = false;
    qint32 index = 0;
    QString newName;
    filename = filename.replace(".map", "");
    while (!found)
    {
        newName = filename + "+" + QString::number(index) + ".map";
        found = !QFile::exists(newName);
        index++;
    }
    return newName;
}

void Multiplayermenu::loadMultiplayerMap()
{    
    m_pMapSelectionView->getCurrentMap()->getGameScript()->init();
    m_pMapSelectionView->updateMapData();
    showPlayerSelection();
}

void Multiplayermenu::initClientGame(quint64, QDataStream &stream)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    spGameMap pMap = GameMap::getInstance();
    pMap->setVisible(false);
    if (!saveGame)
    {
        pMap->initPlayers();
    }
    quint32 seed;
    stream >> seed;
    for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
    {
        GameEnums::AiTypes aiType = m_pMapSelectionView->getCurrentMap()->getPlayer(i)->getBaseGameInput()->getAiType();
        Console::print("Creating AI for player " + QString::number(i) + " of type " + QString::number(aiType), Console::eDEBUG);
        m_pMapSelectionView->getCurrentMap()->getPlayer(i)->deserializeObject(stream);
        m_pMapSelectionView->getCurrentMap()->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(aiType));
    }
    Mainapp::seed(seed);
    Mainapp::setUseSeed(true);

    if (!saveGame)
    {
        pMap->getGameScript()->gameStart();
    }
    pMap->updateSprites();
    // start game
    m_NetworkInterface->setIsServer(false);
    Console::print("Leaving Map Selection Menue", Console::eDEBUG);
    oxygine::getStage()->addChild(new GameMenue(saveGame, m_NetworkInterface));
    // send game started
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << NetworkCommands::CLIENTINITGAME;
    sendStream << m_NetworkInterface->getSocketID();
    m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    pApp->continueThread();
}

bool Multiplayermenu::existsMap(QString& fileName, QByteArray& hash, QString& scriptFileName, QByteArray& scriptHash)
{
    QString path = QCoreApplication::applicationDirPath() + "/maps";
    if (saveGame)
    {
        path = QCoreApplication::applicationDirPath() + "/savegames";
    }
    QStringList filter;
    filter << "*" + fileName;
    QDirIterator dirIter(path, filter, QDir::Files, QDirIterator::Subdirectories);
    bool found = false;
    while (dirIter.hasNext() && !found)
    {
        dirIter.next();
        QString file = dirIter.fileInfo().absoluteFilePath();
        QFile mapFile(file);
        mapFile.open(QIODevice::ReadOnly);
        QCryptographicHash myHash(QCryptographicHash::Sha3_512);
        myHash.addData(&mapFile);
        mapFile.close();
        QByteArray myHashArray = myHash.result();
        if (hash == myHashArray)
        {
            m_pMapSelectionView->setCurrentMap(new GameMap(file, true, false));
            loadMultiplayerMap();
            found = true;
        }
    }
    if (found  && !saveGame)
    {
        if (!scriptFileName.isEmpty())
        {
            QFile scriptFile(scriptFileName);
            scriptFile.open(QIODevice::ReadOnly);
            QCryptographicHash myHash(QCryptographicHash::Sha3_512);
            myHash.addData(&scriptFile);
            scriptFile.close();
            QByteArray myHashArray = myHash.result();
            if (myHashArray != scriptHash)
            {
                found = false;
            }
        }
    }

    return found;

}

void Multiplayermenu::disconnected(quint64)
{
    if (m_Host)
    {
        // handled in player selection
    }
    else
    {
        disconnectNetwork();
        Console::print("Leaving Map Selection Menue", Console::eDEBUG);
        oxygine::getStage()->addChild(new LobbyMenu());
        addRef();
        oxygine::Actor::detach();
        deleteLater();
    }
}

void Multiplayermenu::slotButtonBack()
{
    if (!m_Host || m_MapSelectionStep == MapSelectionStep::selectMap)
    {
        disconnectNetwork();
        Console::print("Leaving Map Selection Menue", Console::eDEBUG);
        oxygine::getStage()->addChild(new LobbyMenu());
        addRef();
        oxygine::Actor::detach();
        deleteLater();
    }
    else if (m_Host)
    {
        if (m_local)
        {
            disconnectNetwork();
            m_pHostAdresse->setVisible(false);
        }
        MapSelectionMapsMenue::slotButtonBack();
        if (saveGame)
        {
            MapSelectionMapsMenue::slotButtonBack();
            saveGame = false;
            m_pPlayerSelection->setSaveGame(saveGame);
        }
    }
}

void Multiplayermenu::slotButtonNext()
{
    if (m_Host && m_MapSelectionStep == MapSelectionStep::selectRules)
    {
        if (m_local)
        {
            m_pHostAdresse->setVisible(true);
            m_NetworkInterface = new TCPServer();
            m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
            createChat();
            emit m_NetworkInterface->sig_connect("", Settings::getGamePort());
            connectNetworkSlots();
            MapSelectionMapsMenue::slotButtonNext();
        }
        else
        {
            connectNetworkSlots();
            startGameOnServer();
        }
    }
    else
    {
        MapSelectionMapsMenue::slotButtonNext();
    }
}

void Multiplayermenu::connectNetworkSlots()
{
    connect(m_NetworkInterface.get(), &NetworkInterface::sigConnected, this, &Multiplayermenu::playerJoined, Qt::QueuedConnection);
    connect(m_NetworkInterface.get(), &NetworkInterface::recieveData, this, &Multiplayermenu::recieveData, Qt::QueuedConnection);
    connect(m_NetworkInterface.get(), &NetworkInterface::sigDisconnected, this, &Multiplayermenu::disconnected, Qt::QueuedConnection);
}

void Multiplayermenu::startGameOnServer()
{
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << NetworkCommands::LAUNCHGAMEONSERVER;
    Filesupport::writeVectorList(sendStream, Settings::getMods());
    GameMap* pMap = GameMap::getInstance();
    pMap->serializeObject(sendStream);
    m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::ServerHosting, false);

    spDialogConnecting pDialogConnecting = new DialogConnecting(tr("Launching game on server"), 1000 * 60 * 5);
    addChild(pDialogConnecting);
    connect(pDialogConnecting.get(), &DialogConnecting::sigCancel, this, &Multiplayermenu::slotCancelHostConnection, Qt::QueuedConnection);
    connect(this, &Multiplayermenu::sigHostGameLaunched, pDialogConnecting.get(), &DialogConnecting::connected, Qt::QueuedConnection);
    connect(pDialogConnecting.get(), &DialogConnecting::sigConnected, this, &Multiplayermenu::slotHostGameLaunched, Qt::QueuedConnection);
}

void Multiplayermenu::createChat()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_Chat = new Chat(m_NetworkInterface,
                      QSize(Settings::getWidth() - 20, 300),
                      NetworkInterface::NetworkSerives::GameChat);
    m_Chat->setPosition(10, Settings::getHeight() - 360);
    addChild(m_Chat);
    pApp->continueThread();
}

void Multiplayermenu::disconnectNetwork()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_GameStartTimer.stop();
    if (m_NetworkInterface.get() != nullptr)
    {
        if (m_Chat.get())
        {
            m_Chat->detach();
            m_Chat = nullptr;
        }
        if (m_local)
        {
            emit m_NetworkInterface->sig_close();
            m_pPlayerSelection->attachNetworkInterface(nullptr);
            m_NetworkInterface = nullptr;
        }
        else
        {
            emit m_NetworkInterface->sigChangeThread(0, pApp->getNetworkThread());
        }
    }
    pApp->continueThread();
}

bool Multiplayermenu::getGameReady()
{
    bool gameReady = true;
    for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
    {
        GameEnums::AiTypes aiType = m_pPlayerSelection->getPlayerAiType(i);
        if (aiType == GameEnums::AiTypes_Open)
        {
            gameReady = false;
            break;
        }
        else if (aiType == GameEnums::AiTypes_ProxyAi)
        {
            if (m_pPlayerSelection->getReady(i) == false)
            {
                gameReady = false;
                break;
            }
        }
    }
    return gameReady;
}

void Multiplayermenu::startGame()
{
    if (!m_Host)
    {
        markGameReady();
        changeButtonText();
    }
    else if (m_local)
    {
        markGameReady();
        startCountdown();
    }
    else
    {
        markGameReady();
        changeButtonText();
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << NetworkCommands::STARTSERVERGAME;
        m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::markGameReady()
{
    m_pPlayerSelection->setPlayerReady(!m_pPlayerSelection->getPlayerReady());
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << NetworkCommands::CLIENTREADY;
    sendStream << m_pPlayerSelection->getPlayerReady();
    m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
}

void Multiplayermenu::changeButtonText()
{
    if (m_pPlayerSelection->getPlayerReady())
    {
        dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Not Ready"));
    }
    else
    {
        dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Ready"));
    }
}

void Multiplayermenu::startCountdown()
{
    if (m_pPlayerSelection->getPlayerReady())
    {
        counter = 5;
        m_GameStartTimer.stop();
        sendServerReady(false);
    }
    // can we start the game?
    else if (getGameReady())
    {
        sendServerReady(true);
        counter = 5;
        m_GameStartTimer.setInterval(std::chrono::seconds(1));
        m_GameStartTimer.setSingleShot(false);
        m_GameStartTimer.start();
        emit m_Chat->sigSendText(QString::number(counter) + "...");
    }
}

void Multiplayermenu::sendServerReady(bool value)
{
    if (m_NetworkInterface->getIsServer())
    {
        if (value)
        {
            emit m_NetworkInterface.get()->sigPauseListening();
        }
        else
        {
            emit m_NetworkInterface.get()->sigContinueListening();
        }
        QVector<qint32> player;
        for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
        {
            GameEnums::AiTypes aiType = m_pPlayerSelection->getPlayerAiType(i);
            if (aiType != GameEnums::AiTypes_Open &&
                aiType != GameEnums::AiTypes_ProxyAi)
            {
                player.append(i);
            }
        }
        m_pPlayerSelection->setPlayerReady(value);
        m_pPlayerSelection->sendPlayerReady(0, player, value);
    }
}

void Multiplayermenu::countdown()
{
    counter--;
    if (getGameReady())
    {
        emit m_Chat->sigSendText(QString::number(counter) + "...");
        if (counter == 0)
        {
            Console::print("Starting game on server", Console::eDEBUG);
            Mainapp* pApp = Mainapp::getInstance();
            pApp->suspendThread();
            defeatClosedPlayers();
            spGameMap pMap = GameMap::getInstance();
            pMap->setVisible(false);
            if (!saveGame)
            {
                pMap->initPlayersAndSelectCOs();
            }
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << NetworkCommands::INITGAME;
            quint32 seed = QRandomGenerator::global()->bounded(std::numeric_limits<quint32>::max());
            stream << seed;
            for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
            {
                Console::print("AI on server for player " + QString::number(i) + " is " + QString::number(pMap->getPlayer(i)->getBaseGameInput()->getAiType()), Console::eDEBUG);
                pMap->getPlayer(i)->serializeObject(stream);
            }
            Mainapp::seed(seed);
            Mainapp::setUseSeed(true);
            if (!saveGame)
            {
                pMap->getGameScript()->gameStart();
            }
            pMap->updateSprites();
            // start game
            Console::print("Leaving Map Selection Menue", Console::eDEBUG);
            oxygine::getStage()->addChild(new GameMenue(saveGame, m_NetworkInterface));
            QThread::msleep(200);
            emit m_NetworkInterface->sig_sendData(0, data, NetworkInterface::NetworkSerives::Multiplayer, false);
            addRef();
            oxygine::Actor::detach();
            deleteLater();
            pApp->continueThread();
        }
    }
    else
    {
        counter = 5;
        m_GameStartTimer.stop();
        sendServerReady(false);
    }
}
