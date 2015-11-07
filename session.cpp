#include "session.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

Session &Session::getInstance()
{
    static Session session;
    return session;
}

Session &Session::add(const QString &key, const QString &value)
{
    data[key] = value;
    return *this;
}

QString Session::get(const QString &key)
{
    return data.contains(key) ? data[key] : "";
}

bool Session::save(const QString& fileName)
{
    qDebug() << "Save a session to a file " + fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open session file for write";
        return false;
    }

    QDataStream stream(&file);
    stream << data;
    file.close();
    return true;
}

bool Session::load(const QString& fileName)
{
    qDebug() << "Load a session from a file " + fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open session file for read";
        return false;
    }

    QDataStream stream(&file);
    stream >> data;
    file.close();
    return true;
}

Session::Session()
{
}
