#include "subtitlestream.h"

SubtitleStream::SubtitleStream(): MediaStream()
{

}

void SubtitleStream::fromJson(const QJsonObject &jsonData)
{
    MediaStream::fromJson(jsonData);
}
