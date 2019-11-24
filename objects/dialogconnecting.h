#ifndef DIALOGCONNECTING_H
#define DIALOGCONNECTING_H

#include <QObject>
#include <qtimer.h>

#include "oxygine-framework.h"

class DialogConnecting;
typedef oxygine::intrusive_ptr<DialogConnecting> spDialogConnecting;

class DialogConnecting : public QObject, public oxygine::Actor
{
    Q_OBJECT
public:
    explicit DialogConnecting(QString text, qint32 timeoutMs);

signals:
    void sigCancel();
public slots:
    void cancel();
    void connected();
    void timeout();
    void connectionTimeout();
private:
     oxygine::spButton m_CancelButton;
     oxygine::spTextField m_Text;
     QString m_Message;
     QTimer m_Timer;
     QTimer m_TimerConnectionTimeout;
     quint8 counter;
};

#endif // DIALOGCONNECTING_H
