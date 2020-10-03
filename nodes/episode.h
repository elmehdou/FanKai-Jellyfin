#ifndef EPISODE_H
#define EPISODE_H

#include "indexednode.h"

#include <media/mediasource.h>



class Episode : public IndexedNode
{
    Q_OBJECT
    Q_PROPERTY(qint64 runTimeTicks READ getRunTimeTicks NOTIFY runTimeTicksChanged)
    Q_PROPERTY(QList<QObject*> mediaSources READ getMediaSourcesQ NOTIFY mediaSourcesChanged)
    Q_PROPERTY(MediaSource *mediaSource READ getMediaSourceQ NOTIFY mediaSourcesChanged)
    Q_PROPERTY(MediaStream *currentAudioStream READ getCurrentAudioStreamQ WRITE setCurrentAudioStreamQ NOTIFY currentAudioStreamChanged)
    Q_PROPERTY(MediaStream *currentSubtitleStream READ getCurrentSubtitleStreamQ WRITE setCurrentSubtitleStreamQ NOTIFY currentSubtitleStreamChanged)

public:
    Episode();

    void fromJson(const QJsonObject &jsonData) override;

    // INVOKABLES
    Q_INVOKABLE void play();

    // SETTERS - GETTERS
    qint64 getRunTimeTicks() const;
    void setRunTimeTicks(const qint64 &value);

    MediaSource *getMediaSourceQ() const;

    QList<QObject *> getMediaSourcesQ() const;
    QList<Sptr<MediaSource> > getMediaSources() const;
    void setMediaSources(const QList<Sptr<MediaSource> > &value);

    MediaStream *getCurrentAudioStreamQ() const;
    Sptr<MediaStream> getCurrentAudioStream() const;
    void setCurrentAudioStreamQ(MediaStream *value);
    void setCurrentAudioStream(const Sptr<MediaStream> &value);

    MediaStream *getCurrentSubtitleStreamQ() const;
    Sptr<MediaStream> getCurrentSubtitleStream() const;
    void setCurrentSubtitleStreamQ(MediaStream *value);
    void setCurrentSubtitleStream(const Sptr<MediaStream> &value);

    qint64 getPositionTicks() const;
    void setPositionTicks(const qint64 &value);

protected:
    qint64 runTimeTicks;
    qint64 positionTicks;

    QList<Sptr<MediaSource>> mediaSources;

    Sptr<MediaStream> currentAudioStream;
    Sptr<MediaStream> currentSubtitleStream;

signals:
    void runTimeTicksChanged();
    void mediaSourcesChanged();
    void currentAudioStreamChanged();
    void currentSubtitleStreamChanged();
};

#endif // EPISODE_H
