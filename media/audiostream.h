#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include "mediastream.h"



class AudioStream : public MediaStream
{
    Q_OBJECT
public:
    AudioStream();

    void fromJson(const QJsonObject &jsonData);

    QString getPlaySessionId() const;
    void setPlaySessionId(const QString &value);

private:
    QString playSessionId;

signals:
    void playSessionIdChanged();
};

#endif // AUDIOSTREAM_H
