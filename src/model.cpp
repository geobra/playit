#include "model.h"

#include <QFileInfo>

Model::Model(QObject *parent)
    : QAbstractListModel(parent)
{
}


int Model::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return tracks.size();
}

QHash<int, QByteArray> Model::roleNames() const {
    return {
        {Filename, "Filename"},
        {FilePath, "FilePath"},
        {Artist, "Artist"},
        {Title, "Title"},
        {Duration, "Duration"},
        {Active, "Active"},
        {Index, "Index"},
    };
}

QVariant Model::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= tracks.size())
        return {};

    auto it = tracks.at(index.row());

    switch (role) {
        case Filename:
            return it.fileName;
        case FilePath:
            return it.filePath;
        case Artist:
            return it.artist;
        case Title:
            return it.title;
        case Duration:
            return it.durationSeconds;
        case Active:
            return it.active;
        case Index:
            return index.row();
        default:
            return {};
    }
}

QString Model::formatList(const QStringList &list) {
    return list.join(QStringLiteral(", "));
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //qDebug() << "Model::setData. i: " << index << ", v: " << value << ", r: " << role;

    if (!value.canConvert(QMetaType::Bool) || !index.isValid() || index.row() < 0 || index.row() >= tracks.size())
        return false;

    //qDebug() << "Model::setData..... Do it...";

    auto it = tracks.begin() + index.row();
    uint32_t v = value.toBool();
    (*it).active = v;

    //for (auto v: tracks)
    //{
    //    qDebug() << "f: " << v.fileName << ", a: " << v.active;
    //}

    // Emit dataChanged with role
    emit dataChanged(index, index, {role});

    return true;
}

bool Model::setActive(QString media)
{
    qDebug() << "Model::setActive called! " << media;
    QString trackName = QFileInfo(media).fileName();

    int prevActiveIndex = -1;
    int newActiveIndex = -1;

    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks.at(i).active == true)
        {
            prevActiveIndex = i;
        }

        //qDebug() << "compare " << tracks.at(i).fileName;
        if (tracks.at(i).fileName.compare(trackName) == 0)
        {
            newActiveIndex = i;
        }
    }

    if (prevActiveIndex != -1)
    {
        //emit dataChanged(index(changedIndex, 0), index(changedIndex, 0), {Filename, FilePath, Artist, Title, Duration, Active});
        //qDebug() << "set " << prevActiveIndex << " to false";

        QVariant bFalse{false};
        setData(index(prevActiveIndex), bFalse, Active);
    }

    if (newActiveIndex != -1)
    {
        //emit dataChanged(index(changedIndex, 0), index(changedIndex, 0), {Filename, FilePath, Artist, Title, Duration, Active});
        //qDebug() << "set " << newActiveIndex << " to true";

        QVariant bTrue{true};
        setData(index(newActiveIndex), bTrue, Active);
    }

    return true;
}

QString Model::getActive()
{
    qDebug() << "Model::getActive called! ";

    QString returnValue{};

    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks.at(i).active == true)
        {
            returnValue = tracks.at(i).filePath;
            break;
        }
    }

    return returnValue;
}

int Model::getActiveIndex()
{
    qDebug() << "Model::getActiveIndex called! ";

    int returnValue = -1;

    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks.at(i).active == true)
        {
            returnValue = i;
            break;
        }
    }

    return returnValue;
}


bool Model::addData(MediaInfo mi) {

    int newRow = tracks.size();

    //qDebug() << "insert " << mi.fileName << "," << mi.filePath << ", " << mi.artist << ", " << mi.title << ", " << mi.durationSeconds << " to model" << ". size: " << tracks.size();

    beginInsertRows(QModelIndex(), newRow, newRow);
    tracks.push_back(mi);
    endInsertRows();

    emit dataChanged(index(0, 0), index(tracks.size() - 1, 0), {Filename, FilePath, Artist, Title, Duration, Active});

    return true;
}

void Model::clearData()
{
    beginRemoveRows(QModelIndex(), 0, tracks.size());
    tracks.clear();
    endRemoveRows();
}
