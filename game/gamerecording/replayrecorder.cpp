#include "replayrecorder.h"

#include "coreengine/settings.h"
#include "coreengine/interpreter.h"
#include "game/gamemap.h"
#include "game/gameaction.h"
#include "gameinput/basegameinputif.h"
#include "coreengine/filesupport.h"

#include <QByteArray>
#include <QDateTime>

ReplayRecorder::ReplayRecorder()
    : QObject()
{

}

ReplayRecorder::~ReplayRecorder()
{
    if (m_recording)
    {
        _dailyMapPos = m_recordFile.pos();
        _dailyBuffer.seek(0);
        Filesupport::writeByteArray(m_stream, _dailyBuffer.buffer());
        m_recordFile.seek(_countPos);
        m_stream << _count;
        m_recordFile.seek(_posOfDailyMapPos);
        m_stream << _dailyMapPos;
        m_recordFile.flush();
    }
    m_recordFile.close();
    if (playing)
    {
        Interpreter::reloadInterpreter(Interpreter::getRuntimeData());
    }
}

void ReplayRecorder::startRecording()
{
    if (Settings::getRecord())
    {
        spGameMap pMap = GameMap::getInstance();
        // compress script enviroment
        QByteArray data = Interpreter::getRuntimeData().toUtf8();
        data = qCompress(data);
        QString currentDate = QDateTime::currentDateTime().toString("dd-MM-yyyy-hh-mm-ss");
        QString fileName = "data/records/" + pMap->getMapName() + "-" + currentDate + ".rec";
        m_recordFile.setFileName(fileName);
        m_recordFile.open(QIODevice::WriteOnly);
        _dailyBuffer.open(QIODevice::ReadWrite);
        m_stream << static_cast<qint32>(data.size());
        for (qint32 i = 0; i < data.size(); i++)
        {
            m_stream << static_cast<qint8>(data.at(i));
        }
        QStringList mods = Settings::getMods();
        Filesupport::writeVectorList(m_stream, mods);
        _countPos = m_recordFile.pos();
        m_stream << _count;
        _posOfDailyMapPos = m_recordFile.pos();
        m_stream << _dailyMapPos;
        pMap->serializeObject(m_stream);
        m_recordFile.flush();
        m_recording = true;
        _currentDay = pMap->getCurrentDay();
    }
}

void ReplayRecorder::recordAction(GameAction* pAction)
{
    if (m_recording && !pAction->getIsLocal())
    {
        spGameMap pMap = GameMap::getInstance();
        qint32 curDay = pMap->getCurrentDay();
        if (_currentDay != curDay && curDay > 1)
        {
            _currentDay = curDay;
            qint64 curPos = _dailyBuffer.pos();
            qint64 size = 0;
            _dailyStream << size;
            _dailyStream << pMap->getCurrentDay();
            _dailyStream << _count;
            _dailyStream << m_recordFile.pos();
            spGameMap pMap = GameMap::getInstance();
            pMap->serializeObject(_dailyStream);
            qint64 seekPos = _dailyBuffer.pos();
            _dailyBuffer.seek(curPos);
            size = (seekPos - curPos);
            _dailyStream << size;
            _dailyBuffer.seek(seekPos);
        }
        pAction->serializeObject(m_stream);
        m_recordFile.flush();
        _count++;
    }
}

bool ReplayRecorder::loadRecord(QString filename)
{
    m_recordFile.setFileName(filename);
    if (m_recordFile.exists())
    {
        m_recordFile.open(QIODevice::ReadOnly);
        QByteArray envData;
        qint32 size = 0;
        m_stream >> size;
        for (qint32 i = 0; i < size; i++)
        {
            qint8 value = 0;
            m_stream >> value;
            envData.append(value);
        }
        envData = qUncompress(envData);
        _mods = Filesupport::readVectorList<QString, QList>(m_stream);
        if (_mods == Settings::getMods())
        {
            QString interpreterEnvironment(envData);
            Interpreter::reloadInterpreter(interpreterEnvironment);
            m_stream >> _count;
            m_stream >> _dailyMapPos;
            // load map
            _mapPos = m_recordFile.pos();
            GameMap* pMap = new GameMap(m_stream);
            _progress = 0;
            // swap out all ai's / or players with a proxy ai.
            for (qint32 i = 0; i < pMap->getPlayerCount(); i++)
            {
                pMap->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(GameEnums::AiTypes::AiTypes_ProxyAi));
            }
            playing = true;
        }
    }
    return playing;
}

GameAction* ReplayRecorder::nextAction()
{
    if (playing)
    {
        if (!m_stream.atEnd() &&
            m_recordFile.pos() < _dailyMapPos &&
            _progress < _count)
        {
            _progress++;
            GameAction* pAction = new GameAction();
            pAction->deserializeObject(m_stream);
            return pAction;
        }
    }
    return nullptr;
}

void ReplayRecorder::seekToDay(qint32 day)
{
    if (day <= 1)
    {
        seekToStart();
    }
    else
    {
        Mainapp* pApp = Mainapp::getInstance();
        pApp->suspendThread();
        spGameMap pMap = GameMap::getInstance();
        pMap->deleteMap();

        m_recordFile.seek(_dailyMapPos);
        // not needed size of the buffer array
        qint32 bufferSize = 0;
        m_stream >> bufferSize;
        bool found = false;
        while (!found && !m_stream.atEnd())
        {
            qint64 seekPos = m_recordFile.pos();
            qint64 size = 0;
            m_stream >> size;
            qint32 curDay;
            qint32 curCount;
            m_stream >> curDay;
            m_stream >> curCount;
            if (curDay == day)
            {
                qint64 seekPos = 0;
                m_stream >> seekPos;
                _progress = curCount;
                pMap = new GameMap(m_stream);
                for (qint32 i = 0; i < pMap->getPlayerCount(); i++)
                {
                    pMap->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(GameEnums::AiTypes::AiTypes_ProxyAi));
                }
                m_recordFile.seek(seekPos);
                found = true;
            }
            else
            {
                m_recordFile.seek(seekPos + size);
            }
        }
        pApp->continueThread();
    }
}

void ReplayRecorder::seekToStart()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_recordFile.seek(_mapPos);
    spGameMap pMap = GameMap::getInstance();
    pMap->deleteMap();
    new GameMap(m_stream);
    _progress = 0;
    // swap out all ai's / or players with a proxy ai.
    for (qint32 i = 0; i < pMap->getPlayerCount(); i++)
    {
        pMap->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(GameEnums::AiTypes::AiTypes_ProxyAi));
    }
    pApp->continueThread();
}

qint32 ReplayRecorder::getDayFromPosition(qint32 count)
{
    qint64 curPos = m_recordFile.pos();
    m_recordFile.seek(_dailyMapPos);
    // not needed size of the buffer array
    qint32 bufferSize = 0;
    m_stream >> bufferSize;
    bool found = false;
    qint32 rDay = 1;
    while (!found && !m_stream.atEnd())
    {
        qint64 seekPos = m_recordFile.pos();
        qint64 size = 0;
        m_stream >> size;
        qint32 day;
        qint32 curCount;
        m_stream >> day;
        m_stream >> curCount;
        if (curCount < count)
        {
            rDay = day;
            m_recordFile.seek(seekPos + size);
        }
        else
        {
            found = true;
        }
    }
    m_recordFile.seek(curPos);
    return rDay;
}
