#include "Media.h"
#include "Settings.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QMediaMetaData>
#include <QEventLoop>
#include <QUrl>
#include <QDebug>

Media::Media(QObject *parent)
    : QObject(parent) //,currentIndex(-1)
{
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    settings = new Settings(this);
    mprisPlayer_ = new MprisPlayer(this);

    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::positionChanged,
            this, &Media::onPositionChanged);

    connect(player, &QMediaPlayer::durationChanged,
            this, &Media::onDurationChanged);

    connect(player, &QMediaPlayer::mediaStatusChanged,
            this, &Media::onMediaStatusChanged);

    connect(player, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus status)
            {
                if (status == QMediaPlayer::BufferedMedia && pendingStartPositionSeconds > 0)
                {
                    player->setPosition(pendingStartPositionSeconds * 1000);
                    pendingStartPositionSeconds = 0;
                }
            });

    // Mpris user lockscreen play/pause buttons to this Media class
    connect(mprisPlayer_, &MprisPlayer::playbackStatusChanged,
            this, &Media::forwardStatusFromMpris);

    // This media class Status to Mpris dbus
    connect(player, &QMediaPlayer::playbackStateChanged,
            this, &Media::forwardStateToMpris);

    setVolume(100);
}

void Media::forwardStatusFromMpris(const QString& status)
{
    qDebug() << " forwardStatusFromMpris: " << status;

    if (status.compare("Paused") == 0)
    {
        pause();
    }

    if (status.compare("Next") == 0)
    {
        //qDebug() << " call Next: " << status;
        if (player->isPlaying())
        {
            emit requestNext("");
        }
    }

    if (status.compare("Previous") == 0)
    {
        qDebug() << " call Previous: " << status;

        if (player->isPlaying())
        {
            emit requestPrevious("");
        }
    }
}

void Media::forwardStateToMpris(QMediaPlayer::PlaybackState state)
{
    //qDebug() << "forwardStateToMpris" << state;

    switch (state)
    {
    case QMediaPlayer::StoppedState:
        mprisPlayer_->setPlaybackStatus("Stopped");
        break;

    case QMediaPlayer::PlayingState:
        mprisPlayer_->setPlaybackStatus("Playing");
        break;

    case QMediaPlayer::PausedState:
        mprisPlayer_->setPlaybackStatus("Paused");
        break;
    default:
        qDebug() << "default state";
    }
}

QString Media::normalizePath(const QString &path)
{
    if (path.startsWith("file://"))
    {
        QUrl url(path);
        return url.toLocalFile();
    }

    return path;
}

QVector<MediaInfo> Media::scanDirectory(const QString &directoryPath)
{
    emit newFolderScan();

    QString prevPlayedFile{};
    QString normalizedPath = normalizePath(directoryPath);

    QVector<MediaInfo> result;

    QDir dir(normalizedPath);

    QFileInfoList files = dir.entryInfoList(
        QStringList() << "*.mp3",
        QDir::Files,
        QDir::Name | QDir::IgnoreCase
        );

    // update settings, store last file position data
    if (files.size() > 0)
    {
        // save the last position info of that directory in settings
        QString lastPath = settings->getCurrentPath();
        QString lastMedia = settings->getCurrentMedia();
        uint32_t lastPos = settings->getCurrentPosition();
        settings->setFilePositionInfo(lastPath, FilePositionInfo{lastMedia, lastPos});

        // save the new directory in settings
        settings->setCurrentPath(normalizedPath);

        // check if the new directory was used before. Restore last media and position, if available
        FilePositionInfo fp = settings->getFilePositionInfo(normalizedPath);

        //qDebug() << "read dir: prev settings: " << fp.name << ", pos: " << fp.position;

        // FIXME check if file is still there!
        if ( ! fp.name.isEmpty() )
        {
            settings->setCurrentMedia(fp.name);
            settings->setCurrentPosition(fp.position);
            prevPlayedFile = fp.name;
        }
    }

    // generate data for the model
    for (const QFileInfo &fileInfo : files)
    {
        QString filePath = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.fileName();

        MediaInfo info = getMp3Info(filePath);

        if (info.durationSeconds >= 0)
        {
            info.fileName = fileName;
            info.filePath = filePath;
            if (fileName.compare(prevPlayedFile) == 0)
            {
                info.active = true;
            }
            else
            {
                info.active = false;
            }

            result.append(info);

            emit mediaFound(info);

#if 0
            qDebug() << "Inserted:"
                     << info.fileName
                     << "| Artist:" << info.artist
                     << "| Title:" << info.title
                     << "| Duration:" << info.durationSeconds << "seconds";
#endif
        }
    }

    return result;
}

MediaInfo Media::getMp3Info(const QString &filePath)
{
    MediaInfo info;
    info.durationSeconds = -1;

    QEventLoop loop;

    connect(player, &QMediaPlayer::metaDataChanged,
            &loop, &QEventLoop::quit);

    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
    player->pause();

    loop.exec();

    qint64 durationMs = player->duration();

    if (durationMs > 0)
        info.durationSeconds = durationMs / 1000;

    QMediaMetaData meta = player->metaData();

    info.artist = meta.value(QMediaMetaData::ContributingArtist).toString();
    info.title  = meta.value(QMediaMetaData::Title).toString();

    return info;
}

void Media::playFile(const QString &filePath, uint32_t startPositionSeconds)
{
    qDebug() << " in playFile: " << filePath << ", pos: " << startPositionSeconds;

    QString normalized = normalizePath(filePath);
    currentFilePath = normalized;

    //qDebug() << "Playing:" << normalized << "start:" << startPositionSeconds << "seconds";

    settings->setCurrentMedia(QFileInfo(normalized).fileName());
    setPlaybackSpeed(settings->getPlaybackSpeed());

    player->setSource(QUrl::fromLocalFile(normalized));
    player->play();

    if (startPositionSeconds > 0)
    {
        pendingStartPositionSeconds = startPositionSeconds;
    }
    else
    {
        settings->setCurrentPosition(0);
    }

    emit trackChanged(normalized);
}

void Media::play()
{
    QString path{settings->getCurrentPath()};
    QString file{settings->getCurrentMedia()};
    uint32_t pos{settings->getCurrentPosition()};

    QString pf = path + "/" + file;

    //qDebug() << "play from settings: " << pf << ", at " << pos;

    playFile(pf, pos);
}

void Media::setPlaybackSpeed(qreal speed)
{
    player->setPlaybackRate(speed);
    settings->setPlaybackSpeed(speed);
}

void Media::pause()
{
    player->pause();
}

void Media::resume()
{
    player->play();
}

void Media::stop()
{
    settings->setCurrentPosition(0);
    player->stop();
}

void Media::seek(uint32_t seconds)
{
    player->setPosition(seconds * 1000);
}

void Media::setVolume(float volume)
{
    audioOutput->setVolume(volume);
}

#if 0
void Media::setPlaylist(const QStringList &files)
{
    playlist.clear();

    for (const QString &file : files)
        playlist.append(normalizePath(file));

    currentIndex = playlist.isEmpty() ? -1 : 0;
}

void Media::next()
{
    qDebug() << " in next: ";

    if (playlist.isEmpty())
        return;

    qDebug() << " in next: playlist not empty!";

    currentIndex++;

    qDebug() << " in next: next index: " << currentIndex;


    if (currentIndex >= playlist.size())
        currentIndex = 0;

    qDebug() << " in next: play index: " << currentIndex;

    playFile(playlist[currentIndex]);
}

void Media::previous()
{
    qDebug() << " in prevoius: ";

    if (playlist.isEmpty())
        return;

    currentIndex--;

    if (currentIndex < 0)
        currentIndex = playlist.size() - 1;

    playFile(playlist[currentIndex]);
}
#endif

qint64 Media::positionSeconds() const
{
    return player->position() / 1000;
}

qint64 Media::durationSeconds() const
{
    return player->duration() / 1000;
}

void Media::onPositionChanged(qint64 position)
{
    emit positionChanged(position / 1000);
}

void Media::onDurationChanged(qint64 duration)
{
    emit durationChanged(duration / 1000);
}

void Media::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        //qDebug() << "Playback finished:" << currentFilePath;

        emit playbackFinished(currentFilePath);
    }
}
