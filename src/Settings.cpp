#include "Settings.h"

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QStandardPaths>

Settings::Settings(QObject *parent) : QObject(parent)
{

}

QString Settings::getCurrentMedia() const
{
    QString returnValue = "";

    QSettings settings;
    if(settings.value("Media/CurrentMedia").toString() != "NOT_SET")
    {
        returnValue = settings.value("Media/CurrentMedia").toString();
    }

    return returnValue;
}

void Settings::setCurrentMedia(QString qm)
{
    QSettings settings;
    settings.setValue("Media/CurrentMedia", qm);
    emit currentMediaChanged(qm);
}

uint32_t Settings::getCurrentPosition() const
{
    uint32_t returnValue = 0;

    QSettings settings;
    bool ok{false};

    settings.value("Media/CurrentPosition").toUInt(&ok);
    if(ok == true)
    {
        returnValue = settings.value("Media/CurrentPosition").toUInt();
    }

    return returnValue;
}

void Settings::setCurrentPosition(uint32_t qp)
{
    QSettings settings;
    settings.setValue("Media/CurrentPosition", qp);
    emit currentPositionChanged(qp);
}

QString Settings::getCurrentPath() const
{
    QString returnValue = "";

    QSettings settings;
    if(settings.value("Media/CurrentPath").toString() != "NOT_SET")
    {
        returnValue = settings.value("Media/CurrentPath").toString();
    }

    return returnValue;
}

void Settings::setCurrentPath(QString qp)
{
    QSettings settings;
    settings.setValue("Media/CurrentPath", qp);
    emit currentPathChanged(qp);
}

FilePositionInfo Settings::getFilePositionInfo(const QString &path)
{
    QSettings settings;
    QVariant value = settings.value("PrevMedia/" + path);

    if (!value.isValid()) {
        return FilePositionInfo{}; // return empty/default struct
    }

    return variantToFilePositionInfo(value.toMap());
}

void Settings::setFilePositionInfo(const QString &path, const FilePositionInfo &filePositionInfo)
{
    QSettings settings;

    //qDebug() << "PREV: setFilePositionInfo: " << filePositionInfo.name << ", pos: " << filePositionInfo.position;

    QVariantMap map = filePositionInfoToVariant(filePositionInfo);

    settings.setValue("PrevMedia/" + path, map);
}

void Settings::setPlaybackSpeed(qreal speed)
{
    QSettings settings;
    settings.setValue("Media/CurrentPlaybackSpeed", speed);
    emit currentPlaybackSpeedChanged(speed);
}

qreal Settings::getPlaybackSpeed() const
{
    qreal returnValue = 1.0;

    QSettings settings;
    QVariant value = settings.value("Media/CurrentPlaybackSpeed");

    if (value.isValid()) {
        returnValue = value.toReal();
    }

    return returnValue;
}


QVariantMap Settings::filePositionInfoToVariant(const FilePositionInfo &info)
{
    QVariantMap map;
    map["name"] = info.name;
    map["position"] = static_cast<quint32>(info.position);
    return map;
}

FilePositionInfo Settings::variantToFilePositionInfo(const QVariantMap &map)
{
    FilePositionInfo info;
    info.name = map.value("name").toString();
    info.position = map.value("position").toUInt();
    return info;
}
