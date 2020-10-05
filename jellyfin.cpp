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

    QString targetUrl = Jellyfin::webmFileUrl.arg(episodeId, accessToken);

    Sptr<Node> season = episode->getParentNode();
    Sptr<Node> series = season->getParentNode();

    setCurrentSeriesQ(series.data());
    setCurrentSeasonQ(season.data());
    setCurrentEpisodeQ(episode.data());

    QmlLinker::goToPlayer(targetUrl);
    QmlLinker::createNotificationModal(QString("Playing %2").arg(episode->getName()),
                                       QString("Lancement du film: %1").arg(episode->getName()));
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
