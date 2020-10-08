#include "qmllinker.h"

#include <QDebug>
#include <QFile>

QmlLinker::QmlLinker(QQmlApplicationEngine *engine): QObject()
  , playerShow(false)
  , playerFullscreen(false)
  , viewType(ViewType::ListView)
  , volume(50)
{
    QmlLinker::engine = engine;
}

// Serialization
QDataStream &operator<<(QDataStream &stream, const QmlLinker &linker)
{
    stream << linker.volume;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, QmlLinker &linker)
{
    int volume = 50;
    stream >> volume;

    linker.setVolume(volume);

    return stream;
}

void QmlLinker::save(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)){
        qDebug() << "Could not open state file for writing.";
        file.close();
        return;
    }

    qDebug() << "Saving state to file";
    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << *this;

    file.write(data);

    file.close();
}

void QmlLinker::load(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "Could not open state file for writing.";
        file.close();
        return;
    }

    qDebug() << "Loading state to file";
    QDataStream inStream(file.readAll());
    if (inStream.device()->bytesAvailable())
        inStream >> *this;

    file.close();
}


// Method invokers
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

bool QmlLinker::getPlayerShow() const
{
    return playerShow;
}

void QmlLinker::setPlayerShow(bool value)
{
    playerShow = value;
    emit playerShowChanged();
}

bool QmlLinker::getPlayerFullscreen() const
{
    return playerFullscreen;
}

void QmlLinker::setPlayerFullscreen(bool value)
{
    playerFullscreen = value;
    emit playerFullscreenChanged();
}

QmlLinker::ViewType QmlLinker::getViewType() const
{
    return viewType;
}

void QmlLinker::setViewType(const ViewType &value)
{
    viewType = value;
    emit viewTypeChanged();
}

int QmlLinker::getVolume() const
{
    return volume;
}

void QmlLinker::setVolume(int value)
{
    volume = value;
    emit volumeChanged();
}
