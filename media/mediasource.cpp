#include "audiostream.h"
#include "mediasource.h"
#include "subtitlestream.h"
#include "videostream.h"

MediaSource::MediaSource() : QObject()
  , streams()
{

}

void MediaSource::fromJson(const QJsonObject &jsonData)
{
    QList<Sptr<MediaStream>> list;

    QJsonArray streamArray = jsonData.value("MediaStreams").toArray();
    for (const QJsonValue &value : streamArray){
        QJsonObject streamJson = value.toObject();
        QString typeName = streamJson.value("Type").toString();
        MediaStream::Type type = MediaStream::typeFromString(typeName);

        Sptr<MediaStream> stream = nullptr;
        switch (type) {
        case MediaStream::Type::Video:
            stream = Sptr<VideoStream>::create(); break;
        case MediaStream::Type::Audio:
            stream = Sptr<AudioStream>::create(); break;
        case MediaStream::Type::Subtitle:
            stream = Sptr<SubtitleStream>::create(); break;
        default:
            continue;
        }

        stream->fromJson(streamJson);
        list << stream;
    }

    setStreams(list);
}

QList<Sptr<MediaStream> > MediaSource::getStreams() const
{
    return streams;
}

void MediaSource::setStreams(const QList<Sptr<MediaStream> > &value)
{
    streams = value;
    emit streamsChanged();
}

QList<QObject *> MediaSource::getAudioStreamsQ() const
{
    QList<QObject*> list;
    for (Sptr<MediaStream> stream : streams){
        if (stream->getType() == MediaStream::Type::Audio)
            list << stream.data();
    }

    return list;
}

QList<QObject *> MediaSource::getSubtitleStreamsQ() const
{
    QList<QObject*> list;
    for (Sptr<MediaStream> stream : streams){
        if (stream->getType() == MediaStream::Type::Subtitle)
            list << stream.data();
    }

    return list;
}
