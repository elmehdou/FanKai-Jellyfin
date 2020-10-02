#ifndef SUBTITLESTREAM_H
#define SUBTITLESTREAM_H

#include "mediastream.h"



class SubtitleStream : public MediaStream
{
    Q_OBJECT
public:
    SubtitleStream();

    void fromJson(const QJsonObject &jsonData);

private:
    QString deliveryUrl;
};

#endif // SUBTITLESTREAM_H
