#ifndef INDEXEDNODE_H
#define INDEXEDNODE_H

#include "node.h"



class IndexedNode : public Node
{
    Q_OBJECT
    Q_PROPERTY(int indexNumber READ getIndexNumber NOTIFY indexNumberChanged)

public:
    IndexedNode();

    virtual void fromJson(const QJsonObject &jsonData) override;

    int getIndexNumber() const;
    void setIndexNumber(int value);

protected:
    int indexNumber;

signals:
    void indexNumberChanged();
};

#endif // INDEXEDNODE_H
