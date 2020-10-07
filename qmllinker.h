#ifndef QMLLINKER_H
#define QMLLINKER_H

#include <QObject>

#include <QQmlApplicationEngine>

class QmlLinker : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool playerShow READ getPlayerShow WRITE setPlayerShow NOTIFY playerShowChanged)
    Q_PROPERTY(bool playerFullscreen READ getPlayerFullscreen WRITE setPlayerFullscreen NOTIFY playerFullscreenChanged)
    Q_PROPERTY(ViewType viewType READ getViewType WRITE setViewType NOTIFY viewTypeChanged)
    Q_PROPERTY(int volume READ getVolume WRITE setVolume NOTIFY volumeChanged)

private:
    explicit QmlLinker(QQmlApplicationEngine *engine);

public:
    inline static QmlLinker *getInstance(QQmlApplicationEngine *engine = nullptr) {
        if (!instance && engine) instance = new QmlLinker(engine);
        return instance;
    }

    enum class ViewType {
        ListView,
        GridView,
    };
    Q_ENUM(ViewType)

    static QObject *getRootObject();

    static void moveToPage(const QString &page);

    static void goToPlayer(const QString &playerUrl);

    static void createMessageModal(const QString &title, const QString &body);

    static void createNotificationModal(const QString &title, const QString &body);

public:

    inline static const QString LoginPage   = "qrc:/qml/Pages/LoginPage.qml";
    inline static const QString MainPage    = "qrc:/qml/Pages/MainPage.qml";

    bool getPlayerShow() const;
    void setPlayerShow(bool value);

    bool getPlayerFullscreen() const;
    void setPlayerFullscreen(bool value);

    ViewType getViewType() const;
    void setViewType(const ViewType &value);

    int getVolume() const;
    void setVolume(int value);

private:
    inline static QmlLinker *instance = nullptr;
    inline static QQmlApplicationEngine *engine = nullptr;

    bool playerShow;
    bool playerFullscreen;
    ViewType viewType;
    int volume;
signals:
    void playerShowChanged();
    void playerFullscreenChanged();
    void viewTypeChanged();
    void volumeChanged();
};

Q_DECLARE_METATYPE(QmlLinker::ViewType)
#endif // QMLLINKER_H
