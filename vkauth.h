#ifndef VKAUTH_H
#define VKAUTH_H

#include <QWebView>
#include <QUrl>
#include <QString>
#include <QList>
#include <QPair>

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
    QWebView browser;
    static const QUrl REDIRECT_URL;
};

#endif // VKAUTH_H
