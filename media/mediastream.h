#ifndef MEDIASTREAM_H
#define MEDIASTREAM_H

#include <QObject>
#include <QMetaEnum>
#include <QJsonObject>
#include <QEnableSharedFromThis>

class MediaStream : public QObject, public QEnableSharedFromThis<MediaStream>
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString displayTitle READ getDisplayTitle NOTIFY displayTitleChanged)
    Q_PROPERTY(int index READ getIndex NOTIFY indexChanged)
    Q_PROPERTY(Type type READ getType NOTIFY typeChanged)

public:
    explicit MediaStream();

    enum class Type{
        None,
        Audio,
        Video,
        Subtitle,
    };
    Q_ENUM(Type)

    inline static Type typeFromString(const QString &typeName){
        return static_cast<MediaStream::Type>(QMetaEnum::fromType<MediaStream::Type>().keyToValue(typeName.toUtf8().data()));
    }
    inline static QString typeToString(MediaStream::Type type){
        return QString::fromUtf8(QMetaEnum::fromType<MediaStream::Type>().valueToKey(static_cast<int>(type)));
    }

    virtual void fromJson(const QJsonObject &jsonData);

    // SETTERS - GETTERS
    QString getTitle() const;
    void setTitle(const QString &value);

    QString getDisplayTitle() const;
    void setDisplayTitle(const QString &value);

    int getIndex() const;
    void setIndex(int value);

    Type getType() const;
    void setType(const Type &value);

private:
    QString title;
    QString displayTitle;
    int index;
    Type type;

signals:
    void titleChanged();
    void displayTitleChanged();
    void indexChanged();
    void typeChanged();
};

Q_DECLARE_METATYPE(MediaStream::Type)
#endif // MEDIASTREAM_H
