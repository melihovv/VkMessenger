#include "longpoll.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "vkrequest.h"
#include "session.h"

LongPoll::LongPoll(QObject *parent)
    : QObject(parent)
{
}

void LongPoll::getLongPollServer()
{
    VkRequest request;
    QByteArray response = request.request(
                "messages.getLongPollServer",
                Params {
                    {"need_pts", "1"},
                    {
                        "access_token",
                        Session::getInstance().get("access_token")
                    },
                    {"v", "5.40"}
                });

    qDebug() << "Get long poll server";
    qDebug() << QString(response);

    if (!response.isEmpty())
    {
        QJsonObject values = QJsonDocument::fromJson(response)
                .object()["response"]
                .toObject();

        key = values["key"].toString();
        serverUrl = values["server"].toString();
        ts = QString::number(values["ts"].toInt());
        pts = QString::number(values["pts"].toInt());
    }
}

void LongPoll::listen()
{
    while (true)
    {
        qDebug() << "Listen";

        VkRequest request;
        QByteArray response = request.longPollRequest(
                    "http://" + serverUrl,
                    Params {
                        {"act", "a_check"},
                        {"key", key},
                        {"ts", ts},
                        {"wait", "25"},
                        {"mode", "0"},
                    });

        qDebug() << QString(response);

        if (!response.isEmpty())
        {
            QJsonObject values = QJsonDocument::fromJson(response)
                    .object();

            ts = QString::number(values["ts"].toInt());

            QJsonArray updates = values["updates"].toArray();
            if (updates.size())
            {
                for (auto update : updates)
                {
                    auto u = update.toArray();
                    if (u[0].toInt() == 4)
                    {
                        int flags = u[2].toInt();
                        int result = !(flags & 2);
                        qDebug() << "Flags: " + QString::number(flags);
                        qDebug() << "Result: " + QString::number(result);

                        if (result)
                        {
                            QString fromId = QString::number(u[3].toInt());
                            QString text = u[6].toString();
                            qDebug() << "New message recieved from " +
                                        fromId + ": " + text;
                            emit messageRecieved(fromId, text);
                        }
                    }
                }
            }
        }
    }
}
