#include "series.h"

#include <jellyfin.h>

Series::Series() : Node()
  , status()
  , productionYear(0)
  , premiereDate()
{
}

void Series::fromJson(const QJsonObject &jsonData)
{
    Node::fromJson(jsonData);

    setStatus(jsonData.value("Status").toString());
    setProductionYear(jsonData.value("ProductionYear").toInt());
    setPremiereDate(QDate::fromString(jsonData.value("PremiereDate").toString()));
}

QString Series::getStatus() const
{
    return status;
}

void Series::setStatus(const QString &value)
{
    status = value;
    emit statusChanged();
}

int Series::getProductionYear() const
{
    return productionYear;
}

void Series::setProductionYear(int value)
{
    productionYear = value;
    emit productionYearChanged();
}

QDate Series::getPremiereDate() const
{
    return premiereDate;
}

void Series::setPremiereDate(const QDate &value)
{
    premiereDate = value;
    emit premiereDateChanged();
}

void Series::updateChildren()
{
    Jellyfin::getInstance()->updateSeasons(sharedFromThis());
    Jellyfin::getInstance()->setCurrentNodeQ(this);
}
