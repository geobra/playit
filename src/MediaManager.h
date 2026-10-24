#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "model.h"
#include "Media.h"
#include "AudioOutputDeviceModel.h"

#include <QObject>


class MediaManager : public QObject
{
    Q_OBJECT
public:
    MediaManager(Model* model, Media* media, QObject *parent = nullptr);
    AudioOutputDeviceModel *audioOutputDevices() const;

signals:

private slots:
    void setAudioOutputDevice(const QAudioDevice &device);
    void receiveMediaInfo(MediaInfo mi);
    void startNextMedia(QString prevMedia);
    void startPrevMedia(QString currentMedia);
    void persistCurrentPosition(uint32_t seconds);


private:
    Model* model_;
    Media* media_;
    AudioOutputDeviceModel *audioOutputDevices_;

    QVector<MediaInfo> mediaInfoVector_;
};

#endif // MEDIAMANAGER_H
