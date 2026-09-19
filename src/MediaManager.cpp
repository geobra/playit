#include "MediaManager.h"
#include "Settings.h"

#include <utility>
#include <QDebug>

MediaManager::MediaManager(Model* model, Media* media, QObject *parent)
    : QObject{parent}
{
    model_ = std::move(model);
    media_ = std::move(media);

    connect(media_, SIGNAL(mediaFound(MediaInfo)), this, SLOT(receiveMediaInfo(MediaInfo)));

    connect(media_, SIGNAL(playbackFinished(QString)), this, SLOT(startNextMedia(QString)));

    connect(media_, SIGNAL(positionChanged(uint32_t)), this, SLOT(persistCurrentPosition(uint32_t)));

    connect(media_, &Media::trackChanged, model_, &Model::setActive);

    connect(media_, &Media::newFolderScan, model_, &Model::clearData);

    connect(media_, SIGNAL(requestNext(QString)), this, SLOT(startNextMedia(QString)));
    connect(media_, SIGNAL(requestPrevious(QString)), this, SLOT(startPrevMedia(QString)));

    // reload last played media and set status, if any
    Settings s;
    QString lastPath = s.getCurrentPath();
    if (! lastPath.isEmpty())
    {
        media_->scanDirectory(lastPath);
    }

    QString lastMedie = s.getCurrentMedia();
    if (! lastMedie.isEmpty())
    {
        model_->setActive(lastMedie);
    }

}

void MediaManager::persistCurrentPosition(uint32_t seconds)
{
    //qDebug() << "MediaManager::persistCurrentPosition " << seconds;
    if (seconds % 10 == 0)
    {
        Settings s;
        s.setCurrentPosition(seconds);
    }
}

void MediaManager::receiveMediaInfo(MediaInfo mi)
{
    mediaInfoVector_.append(mi);

    model_->addData(mi);
}

void MediaManager::startNextMedia(QString prevMedia)
{
    qDebug() << "startNextMedia1: " << prevMedia;
    if (prevMedia.isEmpty())
    {
        prevMedia = model_->getActive();
        qDebug() << "startNextMedia2: " << prevMedia;

    }
    // search for pos in mediaInfoVector. Search for following media and start playing it.
    //qDebug() << "pre media: " << prevMedia;

    bool useNext{false};
    QString nextMedia{};

    for (auto m: mediaInfoVector_)
    {
        //qDebug() << "next? check " << m.filePath << " against " << prevMedia;

        if (useNext)
        {
            nextMedia = m.filePath;
            //qDebug() << "found next media: " << nextMedia;
            break;
        }

        if (m.filePath.compare(prevMedia) == 0)
        {
            useNext = true;
        }
    }

    if (! nextMedia.isEmpty())
    {
        media_->playFile(nextMedia);
    }
}

void MediaManager::startPrevMedia(QString currentMedia)
{
    qDebug() << "starPrevMedia: " << currentMedia;
    if (currentMedia.isEmpty())
    {
        currentMedia = model_->getActive();
        qDebug() << "starPrevMedia: " << currentMedia;

    }
    // search for pos in mediaInfoVector. Search for following media and start playing it.
    QString prevMediaToPlay{};

    for (auto m: mediaInfoVector_)
    {
        //qDebug() << "next? check " << m.filePath << " against " << currentMedia;

        if (m.filePath.compare(currentMedia) == 0)
        {
            break;
        }

        prevMediaToPlay = m.filePath;
    }

    if (! prevMediaToPlay.isEmpty())
    {
        media_->playFile(prevMediaToPlay);
    }
}
