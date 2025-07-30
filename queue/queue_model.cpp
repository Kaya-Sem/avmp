#include "queue_model.hpp"
#include <QFileInfo>

QueueModel::QueueModel(QObject *parent) : QAbstractListModel(parent) {
}

int QueueModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return tracks.size();
}

QVariant QueueModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= tracks.size()) {
        return QVariant();
    }
    
    const auto &track = tracks.at(index.row());
    
    switch (role) {
        case Qt::DisplayRole:
            // Display format: "Artist - Title"
            if (!track->artist.empty() && !track->title.empty()) {
                return QString::fromStdString(track->artist + " - " + track->title);
            } else if (!track->title.empty()) {
                return QString::fromStdString(track->title);
            } else {
                // Fallback to filename
                QFileInfo fileInfo(QString::fromStdString(track->fullPath));
                return fileInfo.baseName();
            }
        default:
            return QVariant();
    }
}

QString QueueModel::getTrackTitle(int index) const {
    if (index < 0 || index >= tracks.size()) {
        return QString();
    }
    
    const auto &track = tracks.at(index);
    if (!track->title.empty()) {
        return QString::fromStdString(track->title);
    } else {
        QFileInfo fileInfo(QString::fromStdString(track->fullPath));
        return fileInfo.baseName();
    }
}

QString QueueModel::getTrackArtist(int index) const {
    if (index < 0 || index >= tracks.size()) {
        return QString();
    }
    
    return QString::fromStdString(tracks.at(index)->artist);
}

QString QueueModel::getTrackAlbum(int index) const {
    if (index < 0 || index >= tracks.size()) {
        return QString();
    }
    
    return QString::fromStdString(tracks.at(index)->album);
}

QString QueueModel::getTrackPath(int index) const {
    if (index < 0 || index >= tracks.size()) {
        return QString();
    }
    
    return QString::fromStdString(tracks.at(index)->fullPath);
}

std::shared_ptr<Track> QueueModel::getTrack(int index) const {
    if (index < 0 || index >= tracks.size()) {
        return nullptr;
    }
    
    return tracks.at(index);
}

void QueueModel::appendTrack(std::shared_ptr<Track> track) {
    beginInsertRows(QModelIndex(), tracks.size(), tracks.size());
    tracks.append(track);
    endInsertRows();
}

void QueueModel::insertTrack(int index, std::shared_ptr<Track> track) {
    if (index < 0 || index > tracks.size()) {
        return;
    }
    
    beginInsertRows(QModelIndex(), index, index);
    tracks.insert(index, track);
    endInsertRows();
}

void QueueModel::removeTrack(int index) {
    if (index < 0 || index >= tracks.size()) {
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    tracks.removeAt(index);
    endRemoveRows();
}

void QueueModel::clear() {
    beginResetModel();
    tracks.clear();
    endResetModel();
}

const QList<std::shared_ptr<Track>>& QueueModel::getTrackList() const {
    return tracks;
} 