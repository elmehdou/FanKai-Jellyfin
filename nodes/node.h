#ifndef NODE_H
#define NODE_H

#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QMetaEnum>
#include <QSharedPointer>
#include <QSet>


template <typename T>
using Sptr = QSharedPointer<T>;

class Node : public QObject, public QEnableSharedFromThis<Node>
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(Type type READ getType NOTIFY typeChanged)
    Q_PROPERTY(bool isFolder READ getIsFolder NOTIFY isFolderChanged)
    Q_PROPERTY(Node *parentNode READ getParentNodeQ NOTIFY parentNodeChanged)
    Q_PROPERTY(QList<QObject*> childrenNodes READ getChildrenNodesQ NOTIFY childrenNodesChanged)

public:
    explicit Node();

    enum class Type{
        None,
        Series,
        Season,
        Episode,
    };
    Q_ENUM(Type)

    inline static Type typeFromString(const QString &typeName){
        return static_cast<Node::Type>(QMetaEnum::fromType<Node::Type>().keyToValue(typeName.toUtf8().data()));
    }
    inline static QString typeToString(Node::Type type){
        return QString::fromUtf8(QMetaEnum::fromType<Node::Type>().valueToKey(static_cast<int>(type)));
    }

    virtual void fromJson(const QJsonObject &jsonData);

    // GETTERS SETTERS
    QString getId() const;
    void setId(const QString &value);

    QString getName() const;
    void setName(const QString &value);

    Node::Type getType() const;
    void setType(Node::Type value);

    bool getIsFolder() const;
    void setIsFolder(bool value);

    QVariantMap getImageTags() const;
    void setImageTags(const QVariantMap &value);

    Node *getParentNodeQ() const;
    Sptr<Node> getParentNode() const;
    void setParentNode(const Sptr<Node> &value);

    QList<QObject *> getChildrenNodesQ() const;
    QList<Sptr<Node> > getChildrenNodes() const;
    void setChildrenNodes(const QList<Sptr<Node> > &value);

    // INVOKABLES
    Q_INVOKABLE QString getPrimaryImage(int width, int height) const;

    Q_INVOKABLE virtual void updateChildren();

protected:
    QString id;
    QString name;
    Type type;
    bool isFolder;
    QVariantMap imageTags;

    Sptr<Node> parentNode;
    QList<Sptr<Node>> childrenNodes;

signals:
    void idChanged();
    void nameChanged();
    void typeChanged();
    void isFolderChanged();
    void imageTagsChanged();
    void parentNodeChanged();
    void childrenNodesChanged();
};

Q_DECLARE_METATYPE(Node::Type)
#endif // NODE_H
