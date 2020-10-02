#ifndef MEDIASOURCE_H
#define MEDIASOURCE_H

#include "mediastream.h"

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QSharedPointer>

template <typename T>
using Sptr = QSharedPointer<T>;

class MediaSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> audioStreams READ getAudioStreamsQ NOTIFY streamsChanged)
    Q_PROPERTY(QList<QObject*> subtitleStreams READ getSubtitleStreamsQ NOTIFY streamsChanged)

public:
    explicit MediaSource();

    void fromJson(const QJsonObject &jsonData);

    // SETTERS - GETTERS
    QList<Sptr<MediaStream> > getStreams() const;
    void setStreams(const QList<Sptr<MediaStream> > &value);

    QList<QObject *> getAudioStreamsQ() const;

    QList<QObject *> getSubtitleStreamsQ() const;

private:
    QList<Sptr<MediaStream>> streams;

signals:
    void streamsChanged();
};

#endif // MEDIASOURCE_H
