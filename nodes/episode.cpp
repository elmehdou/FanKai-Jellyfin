#include "episode.h"
#include <jellyfin.h>
#include <media/audiostream.h>

Episode::Episode(): IndexedNode()
  , runTimeTicks(0)
  , positionTicks(0)
  , mediaSources()
  , currentAudioStream(nullptr)
  , currentSubtitleStream(nullptr)
{

}

void Episode::fromJson(const QJsonObject &jsonData)
{
    IndexedNode::fromJson(jsonData);

    setRunTimeTicks(jsonData.value("RunTimeTicks").toVariant().toLongLong());
}

void Episode::play()
{
    Jellyfin::getInstance()->play(sharedFromThis().dynamicCast<Episode>());
}

qint64 Episode::getRunTimeTicks() const
{
    return runTimeTicks;
}

void Episode::setRunTimeTicks(const qint64 &value)
{
    runTimeTicks = value;
    emit runTimeTicksChanged();
}

MediaSource *Episode::getMediaSourceQ() const
{
    if (mediaSources.count()) return mediaSources.at(0).data();

    return nullptr;
}

QList<QObject *> Episode::getMediaSourcesQ() const
{
    QList<QObject *> list;
    for (Sptr<MediaSource> source : mediaSources){
        list << source.data();
    }

    return list;
}

QList<Sptr<MediaSource> > Episode::getMediaSources() const
{
    return mediaSources;
}

void Episode::setMediaSources(const QList<Sptr<MediaSource> > &value)
{
    mediaSources = value;
    emit mediaSourcesChanged();
}

MediaStream *Episode::getCurrentAudioStreamQ() const
{
    return currentAudioStream.data();
}

Sptr<MediaStream> Episode::getCurrentAudioStream() const
{
    return currentAudioStream;
}

void Episode::setCurrentAudioStreamQ(MediaStream *value)
{
    setCurrentAudioStream(value->sharedFromThis());
}

void Episode::setCurrentAudioStream(const Sptr<MediaStream> &value)
{
    currentAudioStream = value;
    emit currentAudioStreamChanged();
}

MediaStream *Episode::getCurrentSubtitleStreamQ() const
{
    return currentSubtitleStream.data();
}

Sptr<MediaStream> Episode::getCurrentSubtitleStream() const
{
    return currentSubtitleStream;
}

void Episode::setCurrentSubtitleStreamQ(MediaStream *value)
{
    setCurrentSubtitleStream(value->sharedFromThis());
}

void Episode::setCurrentSubtitleStream(const Sptr<MediaStream> &value)
{
    currentSubtitleStream = value;
    emit currentSubtitleStreamChanged();
}

qint64 Episode::getPositionTicks() const
{
    return positionTicks;
}

void Episode::setPositionTicks(const qint64 &value)
{
    positionTicks = value;
}
