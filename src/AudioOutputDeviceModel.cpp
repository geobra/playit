#include "AudioOutputDeviceModel.h"

#include <QDebug>

AudioOutputDeviceModel::AudioOutputDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&mediaDevices_, &QMediaDevices::audioOutputsChanged,
            this, &AudioOutputDeviceModel::refreshDevices);
    refreshDevices();
}

int AudioOutputDeviceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : devices_.size();
}

QVariant AudioOutputDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= devices_.size()) {
        return {};
    }

    const QAudioDevice &device = devices_.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
    case DeviceNameRole:
        return device.description();
    case IsDefaultRole:
        return !defaultDevice_.id().isEmpty() && device.id() == defaultDevice_.id();
    case IsSelectedRole:
        return !selectedDevice_.id().isEmpty() && device.id() == selectedDevice_.id();
    default:
        return {};
    }
}

QHash<int, QByteArray> AudioOutputDeviceModel::roleNames() const
{
    return {
        {DeviceNameRole, "deviceName"},
        {IsDefaultRole, "isDefault"},
        {IsSelectedRole, "isSelected"}
    };
}

QAudioDevice AudioOutputDeviceModel::selectedDevice() const
{
    return selectedDevice_;
}

void AudioOutputDeviceModel::selectDevice(int index)
{
    if (index < 0 || index >= devices_.size()) {
        qWarning() << "Cannot select audio output device at invalid index" << index;
        return;
    }

    const QAudioDevice device = devices_.at(index);
    if (device.id() == selectedDevice_.id()) {
        return;
    }

    selectedDevice_ = device;
    if (!devices_.isEmpty()) {
        emit dataChanged(this->index(0), this->index(devices_.size() - 1),
                         {IsSelectedRole});
    }
    emit selectedAudioDeviceChanged(selectedDevice_);
}

void AudioOutputDeviceModel::refreshDevices()
{
    const QByteArray previousSelectedId = selectedDevice_.id();

    beginResetModel();
    devices_ = QMediaDevices::audioOutputs();
    defaultDevice_ = QMediaDevices::defaultAudioOutput();

    int selectedIndex = indexOfDevice(previousSelectedId);
    if (selectedIndex < 0) {
        selectedIndex = indexOfDevice(defaultDevice_.id());
    }
    selectedDevice_ = selectedIndex >= 0 ? devices_.at(selectedIndex) : QAudioDevice{};
    endResetModel();

    if (selectedDevice_.id() != previousSelectedId) {
        emit selectedAudioDeviceChanged(selectedDevice_);
    }
}

int AudioOutputDeviceModel::indexOfDevice(const QByteArray &id) const
{
    if (id.isEmpty()) {
        return -1;
    }

    for (int index = 0; index < devices_.size(); ++index) {
        if (devices_.at(index).id() == id) {
            return index;
        }
    }
    return -1;
}
