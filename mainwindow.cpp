#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "session.h"
#include "vkrequest.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    vkAuth(this),
    longPoll(this)
{
    ui->setupUi(this);
    ui->input->setEnabled(false);

    connections();

    Session& s = Session::getInstance();
    s.load();
    QString token = s.get("access_token");

    if (s.get("access_token").isEmpty() ||
           !VkAuth::isTokenValid(token))
    {
        vkAuth.loadLoginPage();
    }
    else
    {
        onSuccessLogin();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Session::getInstance().save();
    event->accept();
}

void MainWindow::onSuccessLogin()
{
    Session& s = Session::getInstance();
    loadUserInfo();
    ui->name->setText(s.get("first_name") + " " + s.get("last_name"));

    loadDialogs();
    foreach (QString key, dialogs.keys())
    {
        ui->users->addItem(dialogs[key], key);
    }

    longPoll.getLongPollServer();
    this->showFullScreen();
    longPoll.listen();
}

void MainWindow::onFailuredLogin()
{
    QMessageBox::critical(this,
                          "Неуспешная авторизация",
                          "Не удалось пройти авторизацию. Попробуйте еще раз.");
    this->close();
}

void MainWindow::onOpponentChanged(int index)
{
    ui->dialog->clear();
    QVariant variant = ui->users->itemData(index);

    if (variant.isValid())
    {
        ui->input->setEnabled(true);

        QString id = variant.toString();
        Session &s = Session::getInstance();

        VkRequest request;
        QByteArray response = request.request(
                    "messages.getHistory",
                    Params {
                        {"user_id", id},
                        {
                            "access_token",
                            s.get("access_token")
                        },
                        {"v", "5.37"}
                    });

        qDebug() << "Messages";
        qDebug() << QString(response);

        if (!response.isEmpty())
        {
            QJsonArray messages = QJsonDocument::fromJson(response)
                    .object()["response"]
                    .toObject()["items"]
                    .toArray();

            for (auto message : messages)
            {
                QJsonObject m = message.toObject();
                QString from = QString::number(m["from_id"].toInt());

                ui->dialog->addItem(
                            from == id ?
                                dialogs[id] + ": " + m["body"].toString() :
                            s.get("first_name") + " " + s.get("last_name") +
                            ": " + m["body"].toString()
                            );
            }
        }
    }
    else
    {
        ui->input->setEnabled(false);
    }
}

void MainWindow::onInputReturnPressed()
{
    QVariant variant = ui->users->currentData();
    QString text = ui->input->text();

    if (variant.isValid() && !text.trimmed().isEmpty())
    {
        QString id = variant.toString();
        Session &s = Session::getInstance();

        VkRequest request;
        QByteArray response = request.request(
                    "messages.send",
                    Params {
                        {"user_id", id},
                        {"message", text},
                        {
                            "access_token",
                            s.get("access_token")
                        },
                        {"v", "5.40"}
                    });

        qDebug() << "Send message";
        qDebug() << QString(response);

        if (!response.isEmpty() && !response.contains("error"))
        {
            ui->input->clear();
            ui->dialog->insertItem(0,
                                   s.get("first_name") + " " +
                                   s.get("last_name") + ": " +
                                   text);
            ui->statusBar->showMessage("Сообщение отправлено успешно", 3000);
        }
        else
        {
            ui->statusBar->showMessage("Ошибка при отправке сообщения", 3000);
        }
    }
    else
    {
        ui->input->setEnabled(false);
    }
}

void MainWindow::onMessageRecieved(const QString &fromId, const QString &text)
{
    QVariant variant = ui->users->currentData();

    if (variant.isValid())
    {
        QString id = variant.toString();
        qDebug() << "Message is recieved from " + fromId;
        qDebug() << "Current opponent id " + id;

        if (id == fromId)
        {
            ui->dialog->insertItem(0, dialogs[id] + ": " + text);
        }
    }
}

void MainWindow::connections()
{
    connect(&vkAuth, SIGNAL(loginSuccessed()), this, SLOT(onSuccessLogin()));
    connect(&vkAuth, SIGNAL(loginFailured()), this, SLOT(onFailuredLogin()));
    connect(ui->users,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(onOpponentChanged(int)));
    connect(ui->input,
            SIGNAL(returnPressed()),
            this,
            SLOT(onInputReturnPressed()));
    connect(&longPoll,
            SIGNAL(messageRecieved(QString,QString)),
            this,
            SLOT(onMessageRecieved(QString,QString)));
}

void MainWindow::loadUserInfo(const QString& ids)
{
    Session& s = Session::getInstance();

    if (s.get("first_name").isEmpty() ||
            s.get("last_name").isEmpty() ||
            s.get("photo_50").isEmpty())
    {
        qDebug() << "Load user info";

        VkRequest request;
        QByteArray response = request.request(
                    "users.get",
                    Params {
                        {"user_ids", ids.isEmpty() ? s.get("user_id") : ids},
                        {"fields", "photo_50"},
                        {"access_token", s.get("access_token")},
                        {"v", "5.37"}
                    });

        qDebug() << "User info";
        qDebug() << QString(response);

        if (!response.isEmpty())
        {
            QJsonObject values = QJsonDocument::fromJson(response)
                    .object()
                    .value("response")
                    .toArray()[0].toObject();

            foreach (QString key, values.keys())
            {
                s.add(key, values[key].toString());
            }
        }
    }
}

void MainWindow::loadDialogs()
{
    Session& s = Session::getInstance();

    qDebug() << "Load user dialogs";

    VkRequest request;
    QByteArray response = request.request(
                "messages.getDialogs",
                Params {
                    {"count", "20"},
                    {"access_token", s.get("access_token")},
                    {"v", "5.37"}
                });

    qDebug() << "Dialogs";
    qDebug() << QString(response);

    if (!response.isEmpty())
    {
        QJsonArray dialogs = QJsonDocument::fromJson(response)
                .object()["response"]
                .toObject()["items"]
                .toArray();

        QStringList ids;
        for (auto dialog : dialogs)
        {
            QJsonObject message = dialog.toObject()["message"].toObject();
            ids << QString::number(message["user_id"].toInt());
        }

        if (!ids.isEmpty())
        {
            qDebug() << "Load users info";

            VkRequest request;
            QByteArray response = request.request(
                        "users.get",
                        Params {
                            {"user_ids", ids.join(',')},
                            {
                                "access_token",
                                Session::getInstance().get("access_token")
                            },
                            {"v", "5.37"}
                        });

            qDebug() << "Users info";
            qDebug() << QString(response);

            if (!response.isEmpty())
            {
                QJsonArray users = QJsonDocument::fromJson(response)
                        .object()
                        .value("response")
                        .toArray();

                for (auto user : users)
                {
                    QJsonObject userInfo = user.toObject();
                    this->dialogs.insert(
                                QString::number(userInfo["id"].toInt()),
                            userInfo["first_name"].toString() + " " +
                            userInfo["last_name"].toString()
                            );
                }
            }

            qDebug() << this->dialogs;
        }
    }
}
