#include "episode.h"
#include <jellyfin.h>
#include <media/audiostream.h>

Episode::Episode(): IndexedNode()
  , runTimeTicks(0)
  , positionTicks(0)
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

qint64 Episode::getPositionTicks() const
{
    return positionTicks;
}

void Episode::setPositionTicks(const qint64 &value)
{
    positionTicks = value;
}
