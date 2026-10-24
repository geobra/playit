#ifndef AUDIOOUTPUTDEVICEMODEL_H
#define AUDIOOUTPUTDEVICEMODEL_H

#include <QAbstractListModel>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QList>

class AudioOutputDeviceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        DeviceNameRole = Qt::UserRole + 1,
        IsDefaultRole,
        IsSelectedRole
    };

    explicit AudioOutputDeviceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QAudioDevice selectedDevice() const;

    Q_INVOKABLE void selectDevice(int index);

signals:
    void selectedAudioDeviceChanged(const QAudioDevice &device);

private:
    void refreshDevices();
    int indexOfDevice(const QByteArray &id) const;

    QMediaDevices mediaDevices_;
    QList<QAudioDevice> devices_;
    QAudioDevice selectedDevice_;
    QAudioDevice defaultDevice_;
};

#endif // AUDIOOUTPUTDEVICEMODEL_H
