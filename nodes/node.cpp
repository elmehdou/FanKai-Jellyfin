#include "node.h"
#include <jellyfin.h>

Node::Node() : QObject()
  , id()
  , name()
  , type(Type::None)
  , isFolder(false)
  , imageTags()
  , parentNode(nullptr)
{

}

void Node::fromJson(const QJsonObject &jsonData)
{
    setId(jsonData.value("Id").toString());
    setName(jsonData.value("Name").toString());
    setType(Node::typeFromString(jsonData.value("Type").toString()));
    setIsFolder(jsonData.value("IsFolder").toBool());
    setImageTags(jsonData.value("ImageTags").toVariant().toMap());
}

QString Node::getId() const
{
    return id;
}

void Node::setId(const QString &value)
{
    id = value;
    emit idChanged();
}

QString Node::getName() const
{
    return name;
}

void Node::setName(const QString &value)
{
    name = value;
    emit nameChanged();
}

Node::Type Node::getType() const
{
    return type;
}

void Node::setType(Node::Type value)
{
    type = value;
    emit typeChanged();
}

bool Node::getIsFolder() const
{
    return isFolder;
}

void Node::setIsFolder(bool value)
{
    isFolder = value;
    emit isFolderChanged();
}

QVariantMap Node::getImageTags() const
{
    return imageTags;
}

void Node::setImageTags(const QVariantMap &value)
{
    imageTags = value;
    emit imageTagsChanged();
}

Node *Node::getParentNodeQ() const
{
    return parentNode.data();
}

Sptr<Node> Node::getParentNode() const
{
    return parentNode;
}

void Node::setParentNode(const Sptr<Node> &value)
{
    parentNode = value;
    emit parentNodeChanged();
}

QList<QObject *> Node::getChildrenNodesQ() const
{
    QList<QObject*> list;
    for (Sptr<Node> node : childrenNodes){
        list << node.data();
    }

    return list;
}

QList<Sptr<Node> > Node::getChildrenNodes() const
{
    return childrenNodes;
}

void Node::setChildrenNodes(const QList<Sptr<Node> > &value)
{
    childrenNodes = value;
    emit childrenNodesChanged();
}

QString Node::getPrimaryImage(int width, int height) const
{
    QString tag = imageTags.value("Primary").toString();
    if (tag.isEmpty()) return "";

    return Jellyfin::PrimaryImageUrl.arg(id, QString::number(height), QString::number(width), tag);
}

void Node::updateChildren()
{

}
