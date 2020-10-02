#ifndef JELLYFIN_H
#define JELLYFIN_H

#include "user.h"
#include <nodes/episode.h>

#include <QUrl>
#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QSharedPointer>

template <typename T>
using Sptr = QSharedPointer<T>;

class Jellyfin : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool working READ getWorking NOTIFY workingChanged)
    Q_PROPERTY(Node *rootNode READ getRootNodeQ CONSTANT)
    Q_PROPERTY(Node *currentNode READ getCurrentNodeQ WRITE setCurrentNodeQ NOTIFY currentNodeChanged)
    Q_PROPERTY(Node *currentSeries READ getCurrentSeriesQ WRITE setCurrentSeriesQ NOTIFY currentSeriesChanged)
    Q_PROPERTY(Node *currentSeason READ getCurrentSeasonQ WRITE setCurrentSeasonQ NOTIFY currentSeasonChanged)
    Q_PROPERTY(Node *currentEpisode READ getCurrentEpisodeQ WRITE setCurrentEpisodeQ NOTIFY currentEpisodeChanged)

private:
    explicit Jellyfin();

public:
    inline static Jellyfin *getInstance(){
        if (!instance) instance = new Jellyfin();
        return instance;
    }

    Q_INVOKABLE void login(const QString &username, const QString &password, bool rememberUser);

    Q_INVOKABLE void updateSeries();
    Q_INVOKABLE void updateSeasons(Sptr<Node> show);
    Q_INVOKABLE void updateEpisodes(Sptr<Node> show, Sptr<Node> season);

    void updatePlaybackInfo(Sptr<Episode> episode, bool playAfterUpdate = false);
    void play(Sptr<Episode> episode);

    // SETTERS - GETTERS
    QString getAccessToken() const;
    void setAccessToken(const QString &value);

    QString getDeviceId() const;
    void setDeviceId(const QString &value);

    QString getServerId() const;
    void setServerId(const QString &value);

    bool getWorking() const;
    void setWorking(bool value);

    Node *getRootNodeQ() const;

    Node *getCurrentNodeQ() const;
    void setCurrentNodeQ(Node *node);

    Node *getCurrentSeriesQ() const;
    void setCurrentSeriesQ(Node *node);

    Node *getCurrentSeasonQ() const;
    void setCurrentSeasonQ(Node *node);

    Node *getCurrentEpisodeQ() const;
    void setCurrentEpisodeQ(Node *node);


private:
    QString getShowsUrl();
    QString getSeasonsUrl(Sptr<Node> series);
    QString getEpisodeUrl(Sptr<Node> series, Sptr<Node> season);

    template<typename T>
    void updateFromNetworkReply(QNetworkReply *reply, QHash<QString,Sptr<Node>> &targetHash, Sptr<Node> parent);

public:

    inline static const QString LoginUrl = "https://fankai.fr/Users/authenticatebyname";

    // Args: deviceId   - accessToken
    // Args: User id
    // Args: Series id  - User id
    // Args: Series id  - Season id   - User id
    // Args: Episode id - User id     - audioStream
    // Args: Episode id - accessToken - audioStream
    // Args: Item id    - maxHeight   - maxWidth - imageTag
    inline static const QString AccessHeader    = "MediaBrowser Client=\"Jellyfin Web\", Device=\"Web Browser\", DeviceId=\"%1\", Version=\"10.4.3\", Token=\"%2\"";
    inline static const QString SeriesUrl       = "https://fankai.fr/Users/%1/Items?SortBy=SortName&SortOrder=Ascending&IncludeItemTypes=Series&Recursive=true&Fields=PrimaryImageAspectRatio%2CBasicSyncInfo&ImageTypeLimit=1&EnableImageTypes=Primary,Backdrop,Banner,Thumb&StartIndex=0&Limit=100";
    inline static const QString SeasonsUrl      = "https://fankai.fr/Shows/%1/Seasons?userId=%2&Fields=ItemCounts,PrimaryImageAspectRatio,BasicSyncInfo,CanDelete,MediaSourceCount";
    inline static const QString EpisodesUrl     = "https://fankai.fr/Shows/%1/Episodes?seasonId=%2&userId=%3&Fields=ItemCounts,PrimaryImageAspectRatio,BasicSyncInfo,CanDelete,MediaSourceCount,Overview";
    inline static const QString PlaybackInfoUrl = "https://fankai.fr/Items/%1/PlaybackInfo?UserId=%2&AudioStreamIndex=%3";
    inline static const QString m3u8FileUrl     = "https://fankai.fr/videos/%1/master.m3u8?&MediaSourceId=%1&api_key=%2&AudioStreamIndex=%3&PlaySessionId=%4";
    inline static const QString PrimaryImageUrl = "https://fankai.fr/Items/%1/Images/Primary?maxHeight=%2&maxWidth=%3&tag=%4&quality=90";

private:
    inline static Jellyfin *instance = nullptr;

    Sptr<User> user;
    QString accessToken;
    QString deviceId;
    QString serverId;

    bool working;

    Sptr<Node> rootNode;
    Sptr<Node> currentNode;
    Sptr<Node> currentSeries;
    Sptr<Node> currentSeason;
    Sptr<Node> currentEpisode;

    QHash<QString,Sptr<Node>> seriesHash;
    QHash<QString,Sptr<Node>> seasonsHash;
    QHash<QString,Sptr<Node>> episodesHash;

signals:
    void workingChanged();
    void currentNodeChanged();
    void currentSeriesChanged();
    void currentSeasonChanged();
    void currentEpisodeChanged();
};

template<typename T>
void Jellyfin::updateFromNetworkReply(QNetworkReply *reply, QHash<QString, Sptr<Node> > &targetHash, Sptr<Node> parent)
{
    setWorking(false);

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << "[ Data update ] Request response error." << reply->errorString();
        reply->manager()->deleteLater();
        return;
    }

    QJsonObject jsonData = QJsonDocument::fromJson(reply->readAll()).object();
    if (jsonData.isEmpty()){
        qDebug() << "[ Data update ] Json Parsing Error.";
        reply->manager()->deleteLater();
        return;
    }

    QList<Sptr<Node>> children;

    // Update episodesHash
    QJsonArray jsonArray = jsonData.value("Items").toArray();
    for (const QJsonValue &jsonValue : jsonArray){
        Sptr<Node> node = Sptr<T>::create();
        node->fromJson(jsonValue.toObject());
        QString id = node->getId();
        if (!id.isEmpty()){
            episodesHash.insert(id, node);
            node->setParentNode(parent);
            children << node;
        }
    }

    parent->setChildrenNodes(children);

    reply->manager()->deleteLater();
}

#endif // JELLYFIN_H
