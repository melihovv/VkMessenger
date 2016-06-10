#ifndef VKAUTH_H
#define VKAUTH_H

#include <QUrl>
#include <QString>
#include <QList>
#include <QPair>
#include <QtWebEngineWidgets/QWebEngineView>

class VkAuth : public QObject
{
    Q_OBJECT

public:
    explicit VkAuth(QObject *parent = nullptr);

    void loadLoginPage();
    static bool isTokenValid(QString token);

signals:
    void loginSuccessed();
    void loginFailured();

private slots:
    void onUrlChanged(const QUrl &url);
    void onLoadFinished(bool success);

private:
    QWebEngineView browser;
    static const QUrl REDIRECT_URL;
};

#endif // VKAUTH_H
