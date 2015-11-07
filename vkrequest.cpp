#include "vkrequest.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>
#include <QUrlQuery>

VkRequest::VkRequest(QObject *parent)
    : QObject(parent)
{
}

QByteArray VkRequest::request(const QString &methodName, const Params &params)
{
    if (!methodName.isEmpty() && !params.isEmpty())
    {
        QUrlQuery query;
        for (auto param : params)
        {
            query.addQueryItem(param.first, param.second);
        }

        QUrl url("https://api.vk.com/method/" + methodName);
        url.setQuery(query);

        qDebug() << "Method: " + methodName;
        qDebug() << "Query: " + query.toString();

        QNetworkAccessManager manager;
        QNetworkReply* reply = manager.get(QNetworkRequest(url));
        QEventLoop wait;
        connect(&manager,
                SIGNAL(finished(QNetworkReply*)),
                &wait,
                SLOT(quit()));
        QTimer::singleShot(10000, &wait, SLOT(quit()));
        wait.exec();

        QByteArray response(reply->readAll());
        reply->deleteLater();
        return response;
    }
    return "";
}

QByteArray VkRequest::longPollRequest(const QString &path,
                                      const Params &params)
{
    if (!path.isEmpty() && !params.isEmpty())
    {
        QUrlQuery query;
        for (auto param : params)
        {
            query.addQueryItem(param.first, param.second);
        }

        QUrl url(path);
        url.setQuery(query);

        qDebug() << "Url: " + url.toString();

        QNetworkAccessManager manager;
        QNetworkReply *reply = manager.get(QNetworkRequest(url));
        QEventLoop wait;
        connect(&manager,
                SIGNAL(finished(QNetworkReply*)),
                &wait,
                SLOT(quit()));
        QTimer::singleShot(30000, &wait, SLOT(quit()));
        wait.exec();

        QByteArray response(reply->readAll());
        reply->deleteLater();
        return response;
    }
    return "";}
