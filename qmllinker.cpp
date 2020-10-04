#include "qmllinker.h"

#include <QDebug>

QmlLinker::QmlLinker(QQmlApplicationEngine *engine)
{
    QmlLinker::engine = engine;
}

QObject *QmlLinker::getRootObject()
{
    if (engine){
        return engine->rootObjects().at(0);
    }

    return nullptr;
}

void QmlLinker::moveToPage(const QString &page)
{
    if (!engine) {
        qDebug() << "No engine in QmlLinker";
        return;
    }

    QMetaObject::invokeMethod(getRootObject(),
                              "moveToPage",
                              Q_ARG(QVariant, page));
}

void QmlLinker::goToPlayer(const QString &playerUrl)
{
    if (!engine) {
        qDebug() << "No engine in QmlLinker";
        return;
    }

    QMetaObject::invokeMethod(getRootObject(),
                              "goToPlayer",
                              Q_ARG(QVariant, playerUrl));
}

void QmlLinker::createMessageModal(const QString &title, const QString &body)
{
    if (!engine) {
        qDebug() << "No engine in QmlLinker";
        return;
    }

    QMetaObject::invokeMethod(getRootObject(),
                              "createMessageModal",
                              Q_ARG(QVariant, title),
                              Q_ARG(QVariant, body));
}

void QmlLinker::createNotificationModal(const QString &title, const QString &body)
{
    if (!engine) {
        qDebug() << "No engine in QmlLinker";
        return;
    }

    QMetaObject::invokeMethod(getRootObject(),
                              "createNotificationModal",
                              Q_ARG(QVariant, title),
                              Q_ARG(QVariant, body));
}
