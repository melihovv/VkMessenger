#ifndef VKREQUESTSENDER_H
#define VKREQUESTSENDER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QPair>

typedef QList<QPair<QString, QString>> Params;

class VkRequest : public QObject
{
public:
    explicit VkRequest(QObject *parent = nullptr);

    QByteArray request(const QString &methodName, const Params &params);
    QByteArray longPollRequest(const QString &path,
                               const Params &params);
};

#endif // VKREQUESTSENDER_H
