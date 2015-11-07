#ifndef LONGPOLL_H
#define LONGPOLL_H

#include <QObject>

class LongPoll : public QObject
{
    Q_OBJECT

public:
    explicit LongPoll(QObject *parent = nullptr);

    void getLongPollServer();
    void listen();

signals:
    void messageRecieved(const QString &fromId, const QString &text);

private:
    QString key;
    QString serverUrl;
    QString ts;
    QString pts;
    const QString mode = "0";
};

#endif // LONGPOLL_H
