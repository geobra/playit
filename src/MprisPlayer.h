#pragma once

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QVariantMap>

// ========================
// Core Object
// ========================
class MprisPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString playbackStatus READ playbackStatus WRITE setPlaybackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(QVariantMap metadata READ metadata NOTIFY metadataChanged)

public:
    explicit MprisPlayer(QObject *parent = nullptr);

    QString playbackStatus() const;
    QVariantMap metadata() const;

public slots:
    void setPlaybackStatus(const QString &status);

signals:
    void playbackStatusChanged(const QString &status);
    void metadataChanged();
    void pauseRequestedByLockScreen();

private:
    QString m_playbackStatus;
    QVariantMap m_metadata;
};


// ========================
// Player Interface
// ========================
class MprisPlayerAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus)
    Q_PROPERTY(QVariantMap Metadata READ Metadata)

    Q_PROPERTY(bool CanPlay READ CanPlay)
    Q_PROPERTY(bool CanPause READ CanPause)
    Q_PROPERTY(bool CanControl READ CanControl)
    Q_PROPERTY(bool CanGoNext READ CanGoNext)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious)

public:
    explicit MprisPlayerAdaptor(MprisPlayer *player);

    QString PlaybackStatus() const;
    QVariantMap Metadata() const;

    bool CanPlay() const { return true; }
    bool CanPause() const { return true; }
    bool CanControl() const { return true; }

    bool CanGoNext() const { return true; }
    bool CanGoPrevious() const { return true; }

public slots:
    void Play();
    void Pause();
    void Stop();
    void PlayPause();
    void Next();
    void Previous();

signals:
    void PropertiesChanged(const QString &, const QVariantMap &, const QStringList &);

private:
    MprisPlayer *m_player;
};


// ========================
// Root Interface
// ========================
class MprisRootAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

    Q_PROPERTY(bool CanQuit READ CanQuit)
    Q_PROPERTY(bool CanRaise READ CanRaise)
    Q_PROPERTY(QString Identity READ Identity)
    Q_PROPERTY(QString DesktopEntry READ DesktopEntry)


public:
    explicit MprisRootAdaptor(QObject *parent);

    bool CanQuit() const { return true; }
    bool CanRaise() const { return false; }
    QString Identity() const { return "My Qt Player"; }

    QString DesktopEntry() const { return "playit"; }

public slots:
    void Quit();
    void Raise();
};
