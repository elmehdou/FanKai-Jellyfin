#include "user.h"

User::User(QObject *parent) : QObject(parent)
  , id()
  , username()
  , state(State::Disconnected)
{

}

void User::fromJson(const QJsonObject &jsonData)
{
    setId(jsonData.value("Id").toString());
    setUsername(jsonData.value("Name").toString());
}

QString User::getId() const
{
    return id;
}

void User::setId(const QString &value)
{
    id = value;
    emit idChanged();
}

QString User::getUsername() const
{
    return username;
}

void User::setUsername(const QString &value)
{
    username = value;
    emit usernameChanged();
}

User::State User::getState() const
{
    return state;
}

void User::setState(const User::State &value)
{
    state = value;
    emit stateChanged();
}
