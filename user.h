#ifndef USER_H
#define USER_H

#include <QJsonObject>
#include <QObject>

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject *parent = nullptr);

    enum class State{
        Disconnected,
        Connected,
    };

    Q_ENUM(State)

    void fromJson(const QJsonObject &jsonData);

    // SETTERS GETTERS
    QString getId() const;
    void setId(const QString &value);

    QString getUsername() const;
    void setUsername(const QString &value);

    State getState() const;
    void setState(const State &value);

private:
    QString id;
    QString username;
    State state;

signals:
    void idChanged();
    void usernameChanged();
    void stateChanged();

};

Q_DECLARE_METATYPE(User::State)
#endif // USER_H
