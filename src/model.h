#pragma once

#include "Media.h"

#include <QObject>
#include <QAbstractListModel>
#include <QString>
#include <QMap>
#include <QStringList>

class Model : public QAbstractListModel {
    Q_OBJECT

public:
    explicit Model(QObject *parent = nullptr);

    enum Roles {
        Filename = Qt::UserRole,
        FilePath,
        Artist,
        Title,
        Duration,
        Active,
        Index
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setActive(QString media);
    QString getActive();

    int getActiveIndex();
    //Q_INVOKABLE void clearData();


public slots:
    bool addData(MediaInfo mi);
    void clearData();

private:
    //QMap<QString, QString> hosts;
    QVector<MediaInfo> tracks;

    static QString formatList(const QStringList &list);
};
