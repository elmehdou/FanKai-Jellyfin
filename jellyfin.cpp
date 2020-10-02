#include "jellyfin.h"
#include "qmllinker.h"

#include <QGuiApplication>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QDebug>
#include <QFile>

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

void Jellyfin::updatePlaybackInfo(Sptr<Episode> episode, bool playAfterUpdate)
{
    if (accessToken.isEmpty()){
        qDebug() << "[ PlaybackInfo update ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ PlaybackInfo update ] No episode id.";
        return;
    }

    // Audio stream is returned according to a playSessionId
    // which in turn is defined by the index in the PlaybackInfoUrl (dirty hack)
    QString audioStreamIndex = "1";
    Sptr<MediaStream> currentAudioStream = episode->getCurrentAudioStream();
    if (currentAudioStream){
        audioStreamIndex = episode->getCurrentAudioStream()->getIndex();
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
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

        Sptr<AudioStream> audioStream = episode->getCurrentAudioStream().dynamicCast<AudioStream>();


        QList<Sptr<MediaSource>> list = episode->getMediaSources();
        if (!list.count()){
            // First time requesting episode
            // Populate Media sources and streams
            // set the first audio stream as current

            QJsonArray jsonArray = jsonData.value("MediaSources").toArray();
            for (const QJsonValue &value : jsonArray){
                QJsonObject sourceJson = value.toObject();
                Sptr<MediaSource> source = Sptr<MediaSource>::create();
                source->fromJson(sourceJson);
                list << source;
            }

            episode->setMediaSources(list);

            if (!episode->getCurrentAudioStream()){
                for (Sptr<MediaStream> stream : list.at(0)->getStreams()){
                    if (stream->getType() == MediaStream::Type::Audio){
                        audioStream = stream.dynamicCast<AudioStream>();
                        episode->setCurrentAudioStream(stream);
                        break;
                    }
                }
            }
        }

        qDebug() << "PLAY SESSION ID:" << jsonData.value("PlaySessionId").toString() << episode->getCurrentAudioStream() << audioStream;
        if (audioStream){
            // If an audio stream is present set its PlaySessionId
            audioStream->setPlaySessionId(jsonData.value("PlaySessionId").toString());
        }

        if (playAfterUpdate && list.count()) play(episode);
    });

    QString targetUrl = Jellyfin::PlaybackInfoUrl.arg(episodeId, user->getId(), audioStreamIndex);
    QNetworkRequest request = QNetworkRequest(QUrl(targetUrl));
    request.setRawHeader("X-Emby-Authorization", Jellyfin::AccessHeader.arg(deviceId, accessToken).toUtf8());

    manager->get(request);
    setWorking(true);
}

void Jellyfin::play(Sptr<Episode> episode)
{
    if (accessToken.isEmpty()){
        qDebug() << "[ Play media ] Access Token is empty.";
        return;
    }

    QString episodeId = episode->getId();
    if (episodeId.isEmpty()){
        qDebug() << "[ Play media ] No episode id.";
        return;
    }

    qDebug() << "HERE";

    Sptr<AudioStream> audioStream = episode->getCurrentAudioStream().dynamicCast<AudioStream>();

    QString playSessionId = audioStream ? audioStream->getPlaySessionId() : "";
    QString audioStreamIndex    = audioStream ? QString::number(audioStream->getIndex()) : "1";

    QString fileName            = QString("%1.%2.m3u8").arg(episode->getId(), audioStreamIndex);
    QString localFilePath       = QString ("%1/%2/%3").arg("file://", qApp->applicationDirPath(), fileName);
    QFile file(localFilePath);

    if (file.open(QIODevice::ReadOnly)){
        setCurrentEpisodeQ(episode.data());
        QmlLinker::goToPlayer(localFilePath);
        return;
    }

    qDebug() << "HERE";
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        setWorking(false);

        qDebug() << "HERE";
        if (reply->error() != QNetworkReply::NoError){
            qDebug() << "[ Play media ] Request response error." << reply->errorString();
            reply->manager()->deleteLater();
            return;
        }

        // Save m3u8 file
        QByteArray data = reply->readAll();
        qDebug() << data;

        qDebug() << "HERE";
        QString prefix = QString("https://fankai.fr/videos/%1/").arg(episode->getId());
        data = data.insert(data.indexOf("main.m3u8"), prefix);
//        data = data.insert(data.indexOf("master.m3u8"), prefix);

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)){
            qDebug() << "[ Play media ] Error opening m3u8 cache file.";
        } else {
            file.write(data);
        }

        file.close();

        setCurrentEpisodeQ(episode.data());
        QmlLinker::goToPlayer(localFilePath);
    });

    QString targetUrl = Jellyfin::m3u8FileUrl.arg(episodeId, accessToken, audioStreamIndex, playSessionId);
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
