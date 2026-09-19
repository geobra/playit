#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "model.h"
#include "Media.h"

#include <QObject>


class MediaManager : public QObject
{
    Q_OBJECT
public:
    MediaManager(Model* model, Media* media, QObject *parent = nullptr);

signals:

private slots:
    void receiveMediaInfo(MediaInfo mi);
    void startNextMedia(QString prevMedia);
    void startPrevMedia(QString currentMedia);
    void persistCurrentPosition(uint32_t seconds);


private:
    Model* model_;
    Media* media_;

    QVector<MediaInfo> mediaInfoVector_;
};

#endif // MEDIAMANAGER_H
