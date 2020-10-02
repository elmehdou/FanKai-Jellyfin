#include "videostream.h"

VideoStream::VideoStream(): MediaStream()
{

}

void VideoStream::fromJson(const QJsonObject &jsonData)
{
    MediaStream::fromJson(jsonData);
}
