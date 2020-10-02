#include "indexednode.h"

IndexedNode::IndexedNode(): Node()
  , indexNumber(0)
{

}

void IndexedNode::fromJson(const QJsonObject &jsonData)
{
    Node::fromJson(jsonData);

    setIndexNumber(jsonData.value("IndexNumber").toInt());
}

int IndexedNode::getIndexNumber() const
{
    return indexNumber;
}

void IndexedNode::setIndexNumber(int value)
{
    indexNumber = value;
    emit indexNumberChanged();
}
