#include "season.h"

#include <jellyfin.h>

Season::Season(): IndexedNode()
{

}

void Season::fromJson(const QJsonObject &jsonData)
{
    IndexedNode::fromJson(jsonData);
}

void Season::updateChildren()
{
    Jellyfin::getInstance()->updateEpisodes(parentNode->sharedFromThis(), sharedFromThis());
    Jellyfin::getInstance()->setCurrentNodeQ(this);
}
