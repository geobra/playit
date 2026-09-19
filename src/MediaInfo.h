#ifndef MEDIAINFO
#define MEDIAINFO

#include <QString>

struct MediaInfo
{
    QString fileName;
    QString artist;
    QString title;
    int durationSeconds;
};

#endif
