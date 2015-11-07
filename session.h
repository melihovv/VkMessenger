#ifndef SESSION_H
#define SESSION_H

#include <QHash>

class Session
{
public:
    static Session& getInstance();

    Session &add(const QString& key, const QString& value);
    QString get(const QString& key);

    bool save(const QString& fileName = "session.vk");
    bool load(const QString& fileName = "session.vk");

private:
    Session();

    Session(const Session&) = delete;
    Session(const Session&&) = delete;
    Session &operator=(const Session&) = delete;
    Session &operator=(const Session&&) = delete;

    QHash<QString, QString> data;
};

#endif // SESSION_H
