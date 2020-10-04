#ifndef QMLLINKER_H
#define QMLLINKER_H

#include <QObject>

#include <QQmlApplicationEngine>

class QmlLinker : public QObject
{
    Q_OBJECT
private:
    explicit QmlLinker(QQmlApplicationEngine *engine);

public:
    inline static QmlLinker *getInstance(QQmlApplicationEngine *engine = nullptr) {
        if (!instance && engine) instance = new QmlLinker(engine);
        return instance;
    }

    static QObject *getRootObject();

    static void moveToPage(const QString &page);

    static void goToPlayer(const QString &playerUrl);

    static void createMessageModal(const QString &title, const QString &body);

    static void createNotificationModal(const QString &title, const QString &body);

public:

    inline static const QString LoginPage   = "qrc:/qml/Pages/LoginPage.qml";
    inline static const QString MainPage    = "qrc:/qml/Pages/MainPage.qml";

private:
    inline static QmlLinker *instance = nullptr;
    inline static QQmlApplicationEngine *engine = nullptr;

signals:

};

#endif // QMLLINKER_H
