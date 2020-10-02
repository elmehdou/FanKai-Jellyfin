#include "mediastream.h"

MediaStream::MediaStream() : QObject()
  , title()
  , displayTitle()
  , index(0)
  , type(Type::None)
{

}

void MediaStream::fromJson(const QJsonObject &jsonData)
{
    setTitle(jsonData.value("Title").toString());
    setDisplayTitle(jsonData.value("DisplayTitle").toString());
    setIndex(jsonData.value("Index").toInt());
    setType(MediaStream::typeFromString(jsonData.value("Type").toString()));
}

QString MediaStream::getTitle() const
{
    return title;
}

void MediaStream::setTitle(const QString &value)
{
    title = value;
    emit titleChanged();
}

QString MediaStream::getDisplayTitle() const
{
    return displayTitle;
}

void MediaStream::setDisplayTitle(const QString &value)
{
    displayTitle = value;
    emit displayTitleChanged();
}

int MediaStream::getIndex() const
{
    return index;
}

void MediaStream::setIndex(int value)
{
    index = value;
    emit indexChanged();
}

MediaStream::Type MediaStream::getType() const
{
    return type;
}

void MediaStream::setType(const MediaStream::Type &value)
{
    type = value;
    emit typeChanged();
}
