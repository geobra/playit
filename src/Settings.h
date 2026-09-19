#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QUrl>
#include <QStringList>

#include <QMap>
#include <QString>
#include <QVariant>

struct FilePositionInfo {
    QString name;
    uint32_t position;
};


class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString CurrentPath READ getCurrentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QString CurrentMedia READ getCurrentMedia WRITE setCurrentMedia NOTIFY currentMediaChanged)
    Q_PROPERTY(uint32_t CurrentPosition READ getCurrentPosition WRITE setCurrentPosition NOTIFY currentPositionChanged)
    Q_PROPERTY(qreal CurrentPlaybackSpeed READ getPlaybackSpeed WRITE setPlaybackSpeed NOTIFY currentPlaybackSpeedChanged)


public:
    explicit Settings(QObject *parent = nullptr);

    QString getCurrentPath() const;
    uint32_t getCurrentPosition() const;
    QString getCurrentMedia() const;
    qreal getPlaybackSpeed() const;

    FilePositionInfo getFilePositionInfo(const QString &path);

signals:
    void currentPathChanged(QString cp);
    void currentMediaChanged(QString cm);
    void currentPositionChanged(uint32_t seconds);
    void currentPlaybackSpeedChanged(qreal speed);

public slots:
    void setCurrentPath(QString qp);
    void setCurrentMedia(QString qm);
    void setCurrentPosition(uint32_t qp);
    void setFilePositionInfo(const QString &path, const FilePositionInfo &filePositionInfo);
    void setPlaybackSpeed(qreal speed);

private:
    QVariantMap filePositionInfoToVariant(const FilePositionInfo &info);
    FilePositionInfo variantToFilePositionInfo(const QVariantMap &map);

};

#endif // SETTINGS_H
