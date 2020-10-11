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
#include <QTimer>

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
    Q_INVOKABLE void updatePlaybackInfo();

    void play(Sptr<Episode> episode);
    void notifyPlaying();
    void notifyStopped();
    void notifyProgress();

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

    QString getCurrentPlaySessionId() const;
    void setCurrentPlaySessionId(const QString &value);


private:

    template<typename T>
    void updateFromNetworkReply(QNetworkReply *reply, QHash<QString,Sptr<Node>> &targetHash, Sptr<Node> parent);

public:

    inline static const QString LoginUrl = "https://fankai.fr/Users/authenticatebyname";

    // API URLs
    //
    // Args: User id
    //     : Series id  - User id
    //     : Series id  - Season id   - User id
    //     : Episode id - User id     - audioStream - subtitleStream
    //     : Episode id - accessToken - audioStream - playSessionId
    //     : Episode id - accessToken
    //     : Episode id - accessToken - playSessionId
    //     : Item id    - maxHeight   - maxWidth    - imageTag
    //     : NONE
    //     : NONE
    inline static const QString SeriesUrl       = "https://fankai.fr/Users/%1/Items?IncludeItemTypes=Series&Recursive=true&SortBy=SortName&SortOrder=Ascending";
    inline static const QString SeasonsUrl      = "https://fankai.fr/Shows/%1/Seasons?userId=%2&Fields=ItemCounts";
    inline static const QString EpisodesUrl     = "https://fankai.fr/Shows/%1/Episodes?seasonId=%2&userId=%3&SortBy=SortIndexNumber&SortOrder=Ascending&Fields=ItemCounts,MediaSourceCount";
    inline static const QString PlaybackInfoUrl = "https://fankai.fr/Items/%1/PlaybackInfo?UserId=%2&AudioStreamIndex=%3&SubtitleStreamIndex=%4";
    inline static const QString M3u8FileUrl     = "https://fankai.fr/videos/%1/main.m3u8?&MediaSourceId=%1&api_key=%2&AudioStreamIndex=%3&PlaySessionId=%4";
    inline static const QString WebmFileUrl     = "https://fankai.fr/Videos/%1/stream.webm?Static=true&mediaSourceId=%1&api_key=%2&PlaySessionId=%3";
    inline static const QString FileUrl         = "https://fankai.fr/Items/%1/Download?api_key=%2";
    inline static const QString PrimaryImageUrl = "https://fankai.fr/Items/%1/Images/Primary?maxHeight=%2&maxWidth=%3&tag=%4&quality=90";
    inline static const QString PlayingUrl      = "https://fankai.fr/Sessions/Playing";
    inline static const QString StoppedUrl      = "https://fankai.fr/Sessions/Playing/Stopped";
    inline static const QString ProgressUrl     = "https://fankai.fr/Sessions/Playing/Progress";

    // Request data
    // Args: deviceId   - accessToken
    inline static const QString AccessHeader    = "MediaBrowser Client=\"Jellyfin Web\", Device=\"Web Browser\", DeviceId=\"%1\", Version=\"10.4.3\", Token=\"%2\"";

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

    QTimer progressUpdateLoop;
    QString currentPlaySessionId;

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
        QString id = jsonValue.toObject().value("Id").toString();
        Sptr<Node> node = targetHash.value(id);

        // If the node exists in the target hash then update it
        // Else create it and set its parent node and insert it in hash
        if (node) {
            node->fromJson(jsonValue.toObject());
        } else {
            node = Sptr<T>::create();
            node->fromJson(jsonValue.toObject());
            if (!id.isEmpty()){
                targetHash.insert(id, node);
                node->setParentNode(parent);
            }
        }

        children << node;
    }

    parent->setChildrenNodes(children);

    reply->manager()->deleteLater();
}

#endif // JELLYFIN_H
