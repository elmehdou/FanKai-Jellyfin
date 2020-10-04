#ifndef EPISODE_H
#define EPISODE_H

#include "indexednode.h"

#include <media/mediasource.h>



class Episode : public IndexedNode
{
    Q_OBJECT
    Q_PROPERTY(qint64 runTimeTicks READ getRunTimeTicks NOTIFY runTimeTicksChanged)

public:
    Episode();

    void fromJson(const QJsonObject &jsonData) override;

    // INVOKABLES
    Q_INVOKABLE void play();

    // SETTERS - GETTERS
    qint64 getRunTimeTicks() const;
    void setRunTimeTicks(const qint64 &value);

    qint64 getPositionTicks() const;
    void setPositionTicks(const qint64 &value);

protected:
    qint64 runTimeTicks;
    qint64 positionTicks;

signals:
    void runTimeTicksChanged();
    void mediaSourcesChanged();
};

#endif // EPISODE_H
