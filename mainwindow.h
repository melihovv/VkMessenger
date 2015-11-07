#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMap>
#include "vkauth.h"
#include "longpoll.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QMap<QString, QString> DialogsList;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void onSuccessLogin();
    void onFailuredLogin();
    void onOpponentChanged(int index);
    void onInputReturnPressed();
    void onMessageRecieved(const QString &fromId, const QString &text);

private:
    void connections();
    void loadUserInfo(const QString& ids = "");
    void loadDialogs();

    Ui::MainWindow *ui;
    VkAuth vkAuth;
    DialogsList dialogs;
    LongPoll longPoll;
};

#endif // MAINWINDOW_H
