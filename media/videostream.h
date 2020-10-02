#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include "mediastream.h"



class VideoStream : public MediaStream
{
    Q_OBJECT
public:
    VideoStream();

    void fromJson(const QJsonObject &jsonData);
};

#endif // VIDEOSTREAM_H
