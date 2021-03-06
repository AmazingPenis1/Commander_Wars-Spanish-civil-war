#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QLocalServer>
#include <QVector>
#include "network/rxtask.h"
#include "network/txtask.h"

#include "network/NetworkInterface.h"

/**
 * @brief The LocalServer class for connecting a local pipe to the hosted game
 */
class LocalServer : public NetworkInterface
{
    Q_OBJECT
public:
    LocalServer();
    virtual ~LocalServer();
public slots:
    virtual void connectTCP(QString adress, quint16) override;
    virtual void disconnectTCP() override;
    virtual void forwardData(quint64 socketID, QByteArray data, NetworkInterface::NetworkSerives service) override;
    virtual QVector<quint64> getConnectedSockets() override;
    virtual void changeThread(quint64 socketID, QThread* pThread) override;

    void disconnectSocket();
    void onConnect();
    void disconnectClient(quint64 socketID);
    void pauseListening();
    void continueListening();
    /**
     * @brief addSocket
     * @param socket
     */
    void addSocket(quint64 socket);
    /**
     * @brief removeSocket
     * @param socket
     */
    void removeSocket(quint64 socket);
private:
    QVector<spRxTask> pRXTasks;
    QVector<spTxTask> pTXTasks;
    QVector<QLocalSocket*> pTCPSockets;
    QVector<quint64> m_SocketIDs;
    quint64 m_idCounter = 0;
    QLocalServer* pTCPServer{nullptr};
    bool m_gameServer{false};
};

#endif // LOCALSERVER_H
