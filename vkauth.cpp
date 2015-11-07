#include "vkauth.h"
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "session.h"
#include "vkrequest.h"

const QUrl VkAuth::REDIRECT_URL = "https://oauth.vk.com/blank.html";

VkAuth::VkAuth(QObject *parent)
    : QObject(parent),
      browser()
{
    browser.resize(640, 420);
    connect(&browser,
            SIGNAL(urlChanged(const QUrl&)),
            this,
            SLOT(onUrlChanged(const QUrl&)));
    connect(&browser,
            SIGNAL(loadFinished(bool)),
            this,
            SLOT(onLoadFinished(bool)));
}

void VkAuth::loadLoginPage()
{
    QUrlQuery query;
    query.addQueryItem("client_id", "5097846");
    query.addQueryItem("display", "popup");
    query.addQueryItem("scope", "photos,messages,offline");
    query.addQueryItem("redirect_uri", REDIRECT_URL.toString());
    query.addQueryItem("response_type", "token");

    QUrl url("https://api.vk.com/oauth/authorize");
    url.setQuery(query);

    browser.load(url);
}

bool VkAuth::isTokenValid(QString token)
{
    VkRequest request;
    QByteArray response = request.request(
                "users.search",
                Params {
                    {"q", "test"},
                    {"count", "1"},
                    {"access_token", token},
                    {"v", "5.37"}
                });

    qDebug() << "Check if token is valid";
    qDebug() << QString(response);

    return response.isEmpty() ?
                false :
                QJsonDocument::fromJson(response)
                .object()
                .value("error")
                .isUndefined();
}

void VkAuth::onUrlChanged(const QUrl &url)
{
    qDebug() << "VkAuth::Url changed: " << url;

    QUrl changedUrl = url.toString().replace("#","?");
    QUrlQuery query(changedUrl);

    if (query.hasQueryItem("error"))
    {
        qDebug() << "Error: " + query.queryItemValue("error");
        qDebug() << "Error description: " +
                    query.queryItemValue("error_description");
        emit loginFailured();
        browser.close();
        return;
    }
    if (!query.hasQueryItem("access_token"))
    {
        return;
    }

    Session::getInstance()
            .add("access_token", query.queryItemValue("access_token"))
            .add("user_id", query.queryItemValue("user_id"))
            .add("expires_in", query.queryItemValue("expires_in"));

    qDebug() << "Login successed";
    emit loginSuccessed();
}

void VkAuth::onLoadFinished(bool success)
{
    qDebug() << "VkAuth::Auth page loaded with result: " << success;
    if (!success)
    {
        emit loginFailured();
        browser.close();
    }

    QUrl url = browser.url();
    if (url.path() != REDIRECT_URL.path())
    {
        browser.show();
    }
    else
    {
        browser.close();
    }
}
