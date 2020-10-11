#include "jellyfin.h"
#include "qmllinker.h"

#include <QGuiApplication>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QDebug>

#include <nodes/season.h>
#include <nodes/series.h>

#include <media/audiostream.h>
#include <media/mediasource.h>

Jellyfin::Jellyfin() : QObject()
  , user(Sptr<User>::create())
  , accessToken()
  , deviceId("TW96aWxsYS81LjAgKFdpbmRvd3MgTlQgMTAuMDsgV2luNjQ7IHg2NDsgcnY6ODEuMCkgR2Vja28vMjAxMDAxMDEgRmlyZWZveC84MS4wfDE2MDEzOTk5NDk0Njc1") // TEMPORARY
  , serverId()
  , working(false)
  , rootNode(Sptr<Node>::create())
  , currentNode(rootNode)
  , currentSeries(nullptr)
  , currentSeason(nullptr)
  , currentEpisode(nullptr)
  , seriesHash()
  , seasonsHash()
  , episodesHash()
  , progressUpdateLoop()
{
    rootNode->setName("Main menu");

    progressUpdateLoop.setInterval(5000);
    progressUpdateLoop.callOnTimeout(this, &Jellyfin::notifyProgress);
    progressUpdateLoop.start();
}

void Jellyfin::login(const QString &username, const QString &password, bool rememberUser)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        QByteArray responseData = reply->readAll();
//        qDebug() << "[ Login ]" << responseData;

        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Login ] Authentication Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        QJsonObject jsonData = QJsonDocument::fromJson(responseData).object();
        if (jsonData.isEmpty()){
            qDebug() << "[ Login ] Json Parsing Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        qDebug() << "Login Success";

        // Update Jellyfin singleton
        setAccessToken(jsonData.value("AccessToken").toString());
        setServerId(jsonData.value("ServerId").toString());

        // Update User
        user->fromJson(jsonData.value("User").toObject());
        user->setState(User::State::Connected);

        QmlLinker::createNotificationModal("Connexion réussie", QString("Bienvenue %1 sur la version bureau de FanKai Jellyfin !").arg(user->getUsername()));

        // Request show update
        updateSeries();

        manager->deleteLater();

        // Move to main page
        QmlLinker::moveToPage(QmlLinker::MainPage);
    });


    QJsonObject loginJsonData{
        {"Username", username},
        {"Pw", password}
    };

    QByteArray loginData = QJsonDocument(loginJsonData).toJson();
    QNetworkRequest request = QNetworkRequest(QUrl(Jellyfin::LoginUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, loginData.size());
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->post(request, loginData);
}

void Jellyfin::updateSeries()
{
    if (accessToken.isEmpty()){
        qDebug() << "[ Shows update ] Access Token is empty.";
        return;
    }

    qDebug() << "[ Shows update ]" << accessToken;

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        updateFromNetworkReply<Series>(reply, seriesHash, rootNode);
    });

    QString targetUrl = Jellyfin::SeriesUrl.arg(user->getId());
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
    setWorking(true);
}

void Jellyfin::updateSeasons(Sptr<Node> series)
{
    if (accessToken.isEmpty()){
        qDebug() << "[ Seasons update ] Access Token is empty.";
        return;
    }

    QString seriesId = series->getId();
    if (seriesId.isEmpty()){
        qDebug() << "[ Seasons update ] No series id.";
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        updateFromNetworkReply<Season>(reply, seasonsHash, series);
    });

    QString targetUrl = Jellyfin::SeasonsUrl.arg(seriesId, user->getId());
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
    setWorking(true);
}

void Jellyfin::updateEpisodes(Sptr<Node> series, Sptr<Node> season)
{
    if (accessToken.isEmpty()){
        qDebug() << "[ Episodes update ] Access Token is empty.";
        return;
    }

    QString seriesId = series->getId();
    if (seriesId.isEmpty()){
        qDebug() << "[ Episodes update ] No series id.";
        return;
    }

    QString seasonId = season->getId();
    if (seasonId.isEmpty()){
        qDebug() << "[ Episodes update ] No season id.";
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        updateFromNetworkReply<Episode>(reply, episodesHash, season);
    });

    QString targetUrl = Jellyfin::EpisodesUrl.arg(seriesId, seasonId, user->getId());
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
    setWorking(true);
}

void Jellyfin::updatePlaybackInfo()
{
    // Clear current playsessionid
    setCurrentPlaySessionId("");

    Sptr<Episode> episode = currentEpisode.dynamicCast<Episode>();
    if (!episode) return;

    VlcQmlPlayer *player = QmlLinker::getMediaPlayer();
    if (!player) return;

    // Initials checks
    if (accessToken.isEmpty()){
        qDebug() << "[ updatePlaybackInfo update ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ updatePlaybackInfo update ] No episode id.";
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        QByteArray responseData = reply->readAll();

        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Login ] Authentication Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        QJsonObject jsonData = QJsonDocument::fromJson(responseData).object();
        if (jsonData.isEmpty()){
            qDebug() << "[ Login ] Json Parsing Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        // Update PlaySessionId
        setCurrentPlaySessionId(jsonData.value("PlaySessionId").toString());

        manager->deleteLater();
    });

    QString targetUrl = Jellyfin::PlaybackInfoUrl.arg(episodeId, user->getId(), QString::number(player->audioTrack()), QString::number(player->subtitleTrack()));
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
}

void Jellyfin::play(Sptr<Episode> episode)
{
    // Initials checks
    if (accessToken.isEmpty()){
        qDebug() << "[ PlaybackInfo update ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ PlaybackInfo update ] No episode id.";
        return;
    }

    VlcQmlPlayer *player = QmlLinker::getMediaPlayer();
    if (!player) return;

    // Clear current playsessionid
    setCurrentPlaySessionId("");

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        QByteArray responseData = reply->readAll();

        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Login ] Authentication Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        QJsonObject jsonData = QJsonDocument::fromJson(responseData).object();
        if (jsonData.isEmpty()){
            qDebug() << "[ Login ] Json Parsing Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        // Update PlaySessionId
        setCurrentPlaySessionId(jsonData.value("PlaySessionId").toString());

        // Start playing based on PlaySessionId
        QString targetUrl = Jellyfin::WebmFileUrl.arg(episodeId, accessToken, currentPlaySessionId);

        Sptr<Node> season = episode->getParentNode();
        Sptr<Node> series = season->getParentNode();

        notifyStopped();

        setCurrentSeriesQ(series.data());
        setCurrentSeasonQ(season.data());
        setCurrentEpisodeQ(episode.data());

        updatePlaybackInfo();

        QmlLinker::goToPlayer(targetUrl);
        QmlLinker::createNotificationModal(QString("Playing %2").arg(episode->getName()),
                                           QString("Lancement du film: %1").arg(episode->getName()));

        notifyPlaying();

        manager->deleteLater();
    });

    QString targetUrl = Jellyfin::PlaybackInfoUrl.arg(episodeId, user->getId(), QString::number(player->audioTrack()), QString::number(player->subtitleTrack()));
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
}

void Jellyfin::notifyPlaying()
{
    Sptr<Episode> episode = currentEpisode.dynamicCast<Episode>();
    if (!episode) return;

    VlcQmlPlayer *player = QmlLinker::getMediaPlayer();
    if (!player) return;

    // Initials checks
    if (accessToken.isEmpty()){
        qDebug() << "[ postProgress update ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ postProgress update ] No episode id.";
        return;
    }

    int volume = player->volume();
    volume = volume < 0 ? 0 : volume;
    int positionTicks = player->time();
    positionTicks = positionTicks < 0 ? 0 : positionTicks;

    QJsonObject playingJsonData{{"VolumeLevel",            volume },
                                {"IsMuted",                volume ? false : true},
                                {"IsPaused",               player->state() == Vlc::Playing ? false : true},
                                {"RepeatMode",             "RepeatNone"},
                                {"MaxStreamingBitrate",    9225700},
                                {"PositionTicks",          positionTicks},
                                {"PlaybackStartTimeTicks", 16021732543690000},
                                {"SubtitleStreamIndex",    3},
                                {"AudioStreamIndex",       1},
                                {"BufferedRanges",         QJsonArray{}},
                                {"NowPlayingQueue",        QJsonArray{
                                                            QJsonObject{
                                                                {"Id",              episodeId},
                                                                {"PlaylistItemId",  QString("playlistItem%1").arg(QString::number(episode->getIndexNumber()))}}
                                                            }},
                                {"PlayMethod",             "DirectStream"},
                                {"PlaySessionId",          ""},
                                {"PlaylistItemId",         QString("playlistItem%1").arg(QString::number(episode->getIndexNumber()))},
                                {"MediaSourceId",          episodeId},
                                {"CanSeek",                true},
                                {"ItemId",                 episodeId}};

    QByteArray playingData = QJsonDocument(playingJsonData).toJson();

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){

        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Login ] Authentication Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        reply->manager()->deleteLater();
        setWorking(false);
    });

    QString targetUrl = Jellyfin::PlayingUrl;
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, playingData.size());

    manager->post(request, playingData);
    setWorking(true);
}

void Jellyfin::notifyStopped()
{
    // Initials checks
    Sptr<Episode> episode = currentEpisode.dynamicCast<Episode>();
    if (!episode) return;

    if (currentPlaySessionId.isEmpty()) return;

    VlcQmlPlayer *player = QmlLinker::getMediaPlayer();
    if (!player) return;

    if (accessToken.isEmpty()){
        qDebug() << "[ postProgress update ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ postProgress update ] No episode id.";
        return;
    }

    int volume = player->volume();
    volume = volume < 0 ? 0 : volume;
    int positionTicks = player->time();
    positionTicks = positionTicks < 0 ? 0 : positionTicks;

    QJsonObject stoppedJsonData{{"VolumeLevel",               volume },
                                {"IsMuted",                volume ? false : true},
                                {"IsPaused",               player->state() == Vlc::Playing ? false : true},
                                {"RepeatMode",             "RepeatNone"},
                                {"MaxStreamingBitrate",    9225700},
                                {"PositionTicks",          positionTicks},
                                {"PlaybackStartTimeTicks", 16021732543690000},
                                {"SubtitleStreamIndex",    player->subtitleTrack()},
                                {"AudioStreamIndex",       player->audioTrack()},
                                {"BufferedRanges",         QJsonArray{}},
                                {"NowPlayingQueue",        QJsonArray{}},
                                {"PlayMethod",             "Transcode"},
                                {"PlaySessionId",          currentPlaySessionId},
                                {"PlaylistItemId",         QString("playlistItem%1").arg(QString::number(episode->getIndexNumber()))},
                                {"MediaSourceId",          episodeId},
                                {"CanSeek",                true},
                                {"ItemId",                 episodeId},
                                {"NowPlayingQueue",        QJsonArray{
                                                                QJsonObject{
                                                                    {"Id",              episodeId},
                                                                    {"PlaylistItemId",  QString("playlistItem%1").arg(QString::number(episode->getIndexNumber()))}}
                                                                }}
                                };

    QByteArray stoppedData = QJsonDocument(stoppedJsonData).toJson();

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){

        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Login ] Authentication Error.";
            QmlLinker::createMessageModal("Échec de connexion", QString("Nous sommes dans l'impossibilité de nous connecter au serveur sélectionné. Veuillez vérifier qu'il est opérationnel et réessayez."));
            manager->deleteLater();
            return;
        }

        reply->manager()->deleteLater();
        setWorking(false);
    });

    QString targetUrl = Jellyfin::PlayingUrl;
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, stoppedData.size());

    manager->post(request, stoppedData);
    setWorking(true);
}

void Jellyfin::notifyProgress()
{

    // Initials checks
    Sptr<Episode> episode = currentEpisode.dynamicCast<Episode>();
    if (!episode) return;

    if (currentPlaySessionId.isEmpty()) return;

    VlcQmlPlayer *player = QmlLinker::getMediaPlayer();
    if (!player) return;

    if (accessToken.isEmpty()){
        qDebug() << "[ postProgress update ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ postProgress update ] No episode id.";
        return;
    }

    int volume = player->volume();
    volume = volume < 0 ? 0 : volume;
    int positionTicks = player->time();
    positionTicks = positionTicks < 0 ? 0 : positionTicks;

    QJsonObject progressJsonData{{"VolumeLevel",               volume },
                                    {"IsMuted",                volume ? false : true},
                                    {"IsPaused",               player->state() == Vlc::Playing ? false : true},
                                    {"RepeatMode",             "RepeatNone"},
                                    {"MaxStreamingBitrate",    9225700},
                                    {"PositionTicks",          positionTicks},
                                    {"PlaybackStartTimeTicks", 16021732543690000},
                                    {"SubtitleStreamIndex",    player->subtitleTrack()},
                                    {"AudioStreamIndex",       player->audioTrack()},
                                    {"BufferedRanges",         QJsonArray{}},
                                    {"PlayMethod",             "Transcode"},
                                    {"PlaySessionId",          currentPlaySessionId},
                                    {"PlaylistItemId",         QString("playlistItem%1").arg(QString::number(episode->getIndexNumber()))},
                                    {"MediaSourceId",          episodeId},
                                    {"CanSeek",                true},
                                    {"ItemId",                 episodeId},
                                    {"EventName",              "timeupdate"}};

    QByteArray progressData = QJsonDocument(progressJsonData).toJson();

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){

        if (reply->error() != QNetworkReply::NoError){
            reply->manager()->deleteLater();
            return;
        }

        reply->manager()->deleteLater();
        setWorking(false);
    });

    QString targetUrl = Jellyfin::ProgressUrl;
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, progressData.size());

    manager->post(request, progressData);
    setWorking(true);
}

// SETTERS GETTERS

QString Jellyfin::getAccessToken() const
{
    return accessToken;
}

void Jellyfin::setAccessToken(const QString &value)
{
    accessToken = value;
}

QString Jellyfin::getDeviceId() const
{
    return deviceId;
}

void Jellyfin::setDeviceId(const QString &value)
{
    deviceId = value;
}

QString Jellyfin::getServerId() const
{
    return serverId;
}

void Jellyfin::setServerId(const QString &value)
{
    serverId = value;
}

bool Jellyfin::getWorking() const
{
    return working;
}

void Jellyfin::setWorking(bool value)
{
    working = value;
    workingChanged();
}

Node *Jellyfin::getRootNodeQ() const
{
    return rootNode.data();
}

Node *Jellyfin::getCurrentNodeQ() const
{
    return currentNode.data();
}

void Jellyfin::setCurrentNodeQ(Node *node)
{
    currentNode = node->sharedFromThis();
    emit currentNodeChanged();
}

Node *Jellyfin::getCurrentSeriesQ() const
{
    return currentSeries.data();
}

void Jellyfin::setCurrentSeriesQ(Node *node)
{
    currentSeries = node->sharedFromThis();
    emit currentSeriesChanged();
}

Node *Jellyfin::getCurrentSeasonQ() const
{
    return currentSeason.data();
}

void Jellyfin::setCurrentSeasonQ(Node *node)
{
    currentSeason = node->sharedFromThis();
    emit currentSeasonChanged();
}

Node *Jellyfin::getCurrentEpisodeQ() const
{
    return currentEpisode.data();
}

void Jellyfin::setCurrentEpisodeQ(Node *node)
{
    currentEpisode = node->sharedFromThis();
    emit currentEpisodeChanged();
}

QString Jellyfin::getCurrentPlaySessionId() const
{
    return currentPlaySessionId;
}

void Jellyfin::setCurrentPlaySessionId(const QString &value)
{
    currentPlaySessionId = value;
}
