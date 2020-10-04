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
{
    rootNode->setName("Main menu");
}

void Jellyfin::login(const QString &username, const QString &password, bool rememberUser)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        QByteArray responseData = reply->readAll();
//        qDebug() << "[ Login ]" << responseData;

        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Login ] Authentication Error.";
            manager->deleteLater();
            return;
        }

        QJsonObject jsonData = QJsonDocument::fromJson(responseData).object();
        if (jsonData.isEmpty()){
            qDebug() << "[ Login ] Json Parsing Error.";
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

    // M3u8 file is returned according to a playSessionId
    // which in turn is defined by the audio stream index in the PlaybackInfoUrl

    Sptr<AudioStream> currentAudioStream = episode->getCurrentAudioStream().dynamicCast<AudioStream>();
    if (currentAudioStream){
        // Get the audio stream index in case it has already been selected
        // Get playsessionId if any for the current audio stream
        QString audioStreamIndex = QString::number(currentAudioStream->getIndex());
        QString playSessionId = currentAudioStream->getPlaySessionId();

        if (!playSessionId.isEmpty()){
            // Prepare m3u8 file url, set episode as current and set player url
            QString targetUrl = Jellyfin::m3u8FileUrl.arg(episodeId, accessToken, audioStreamIndex, playSessionId);
            setCurrentEpisodeQ(episode.data());
            QmlLinker::goToPlayer(targetUrl);
            return;
        }
    }

    // If no currentAudioStream then the episode has not been loaded yet
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        setWorking(false);

        // Simple error checking
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

        // Get current audio stream since the playSessionId might be the missing info
        Sptr<AudioStream> audioStream = episode->getCurrentAudioStream().dynamicCast<AudioStream>();

        // Check if the mediaSource list is populated
        QList<Sptr<MediaSource>> list = episode->getMediaSources();
        if (!list.count()){

            // First time requesting episode
            // Populate Media sources and streams
            QJsonArray jsonArray = jsonData.value("MediaSources").toArray();
            for (const QJsonValue &value : jsonArray){
                QJsonObject sourceJson = value.toObject();
                Sptr<MediaSource> source = Sptr<MediaSource>::create();
                source->fromJson(sourceJson);
                list << source;
            }
            episode->setMediaSources(list);

            // set the first encountered audio stream as current
            for (Sptr<MediaStream> stream : list.at(0)->getStreams()){
                if (stream->getType() == MediaStream::Type::Audio){
                    audioStream = stream.dynamicCast<AudioStream>();
                    episode->setCurrentAudioStream(stream);
                    break;
                }
            }
        }

        QString playSessionId = jsonData.value("PlaySessionId").toString();
        if (audioStream){
            // If an audio stream is present set its PlaySessionId
            audioStream->setPlaySessionId(playSessionId);
        }

        // Prepare m3u8 file url, set episode as current and set player url
        QString targetUrl = Jellyfin::m3u8FileUrl.arg(episodeId, accessToken, QString::number(audioStream->getIndex()), playSessionId);
        setCurrentEpisodeQ(episode.data());
        QmlLinker::goToPlayer(targetUrl);
    });

    QString targetUrl = Jellyfin::PlaybackInfoUrl.arg(episodeId, user->getId(), "0");
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
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
