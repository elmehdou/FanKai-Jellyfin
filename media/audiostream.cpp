#include "audiostream.h"

AudioStream::AudioStream(): MediaStream()
 , playSessionId()
{

}

void AudioStream::fromJson(const QJsonObject &jsonData)
{
    MediaStream::fromJson(jsonData);
}

QString AudioStream::getPlaySessionId() const
{
    return playSessionId;
}

void AudioStream::setPlaySessionId(const QString &value)
{
    playSessionId = value;
}
