#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <cstdint>

#include "MprisPlayer.h"
#include "Settings.h"

struct MediaInfo
{
    QString fileName;
    QString filePath;
    QString artist;
    QString title;
    int durationSeconds;
    bool active;
};

class Media : public QObject
{
    Q_OBJECT

public:
    explicit Media(QObject *parent = nullptr);

    qint64 positionSeconds() const;
    qint64 durationSeconds() const;

    //void setPlaylist(const QStringList &files);

public slots:

    QVector<MediaInfo> scanDirectory(const QString &directoryPath);

    void playFile(const QString &filePath, uint32_t startPositionSeconds = 0);
    void play(); // starts from last known position in settings
    void pause();
    void resume();
    void stop();
    void setPlaybackSpeed(qreal speed);

    //void next();
    //void previous();

    void seek(uint32_t seconds);

    void setVolume(float volume);

signals:

    void mediaFound(MediaInfo info);

    void positionChanged(uint32_t seconds);
    void durationChanged(uint32_t seconds);
    void trackChanged(QString file);

    void playbackFinished(QString filePath);
    void newFolderScan();

    void requestNext(QString current);
    void requestPrevious(QString current);

private slots:
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void forwardStateToMpris(QMediaPlayer::PlaybackState state);
    void forwardStatusFromMpris(const QString& status);


private:
    QString normalizePath(const QString &path);
    MediaInfo getMp3Info(const QString &filePath);

    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    Settings *settings;
    MprisPlayer *mprisPlayer_;

    QStringList playlist;
    //int currentIndex;

    QString currentFilePath;
    uint32_t pendingStartPositionSeconds{0};
};

#endif
