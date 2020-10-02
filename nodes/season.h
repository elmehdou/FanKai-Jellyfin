#ifndef SEASON_H
#define SEASON_H

#include "indexednode.h"



class Season : public IndexedNode
{
    Q_OBJECT

public:
    Season();

    void fromJson(const QJsonObject &jsonData) override;

    // INVOKABLES
    Q_INVOKABLE void updateChildren() override;

};

#endif // SEASON_H
