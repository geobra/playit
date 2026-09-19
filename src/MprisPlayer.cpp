#include "MprisPlayer.h"
#include <QDBusConnection>
#include <QCoreApplication>

// ========================
// Core
// ========================
MprisPlayer::MprisPlayer(QObject *parent)
    : QObject(parent),
    m_playbackStatus("Stopped")
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    connection.registerService("org.mpris.MediaPlayer2.playit");
    connection.registerObject("/org/mpris/MediaPlayer2", this);

    new MprisPlayerAdaptor(this);
    new MprisRootAdaptor(this);

    // GNOME requires non-empty metadata
    m_metadata["xesam:title"] = "Unknown Track";
    m_metadata["xesam:artist"] = QStringList() << "Unknown Artist";
}

QString MprisPlayer::playbackStatus() const
{
    return m_playbackStatus;
}

QVariantMap MprisPlayer::metadata() const
{
    return m_metadata;
}

void MprisPlayer::setPlaybackStatus(const QString &status)
{
    if (m_playbackStatus == status)
        return;

    m_playbackStatus = status;
    emit playbackStatusChanged(status);
}


// ========================
// Player Adaptor
// ========================
MprisPlayerAdaptor::MprisPlayerAdaptor(MprisPlayer *player)
    : QDBusAbstractAdaptor(player),
    m_player(player)
{
    connect(player, &MprisPlayer::playbackStatusChanged,
            this, [this](const QString &status) {
                QVariantMap changed;
                changed["PlaybackStatus"] = status;

                emit PropertiesChanged("org.mpris.MediaPlayer2.Player",
                                       changed,
                                       {});
            });

    connect(player, &MprisPlayer::metadataChanged,
            this, [this]() {
                QVariantMap changed;
                changed["Metadata"] = m_player->metadata();

                emit PropertiesChanged("org.mpris.MediaPlayer2.Player",
                                       changed,
                                       {});
            });
}

QString MprisPlayerAdaptor::PlaybackStatus() const
{
    return m_player->playbackStatus();
}

QVariantMap MprisPlayerAdaptor::Metadata() const
{
    return m_player->metadata();
}

void MprisPlayerAdaptor::Play()
{
    m_player->setPlaybackStatus("Playing");
}

void MprisPlayerAdaptor::Pause()
{
    m_player->setPlaybackStatus("Paused");
}

void MprisPlayerAdaptor::PlayPause()
{
    const QString current = m_player->playbackStatus();

    if (current == "Playing") {
        m_player->setPlaybackStatus("Paused");
    } else {
        m_player->setPlaybackStatus("Playing");
    }
}

void MprisPlayerAdaptor::Stop()
{
    m_player->setPlaybackStatus("Stopped");
}

void MprisPlayerAdaptor::Next()
{
    m_player->setPlaybackStatus("Next");
}

void MprisPlayerAdaptor::Previous()
{
    m_player->setPlaybackStatus("Previous");
}


// ========================
// Root Adaptor
// ========================
MprisRootAdaptor::MprisRootAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

void MprisRootAdaptor::Quit()
{
    QCoreApplication::quit();
}

void MprisRootAdaptor::Raise()
{
}
