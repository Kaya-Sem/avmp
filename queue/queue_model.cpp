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
        case CurrentTrackRole:
            return index.row() == currentIndex;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> QueueModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[DisplayRole] = "display";
    roles[CurrentTrackRole] = "isCurrentTrack";
    return roles;
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

void QueueModel::insertTrack(std::shared_ptr<Track> track, int index) {
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
    currentIndex = -1;
    endResetModel();
}

void QueueModel::setCurrentIndex(int index) {
    if (index != currentIndex) {
        int oldIndex = currentIndex;
        currentIndex = index;
        
        // Emit dataChanged for both the old and new current tracks
        if (oldIndex >= 0 && oldIndex < tracks.size()) {
            QModelIndex oldModelIndex = createIndex(oldIndex, 0);
            emit dataChanged(oldModelIndex, oldModelIndex, {CurrentTrackRole});
        }
        if (currentIndex >= 0 && currentIndex < tracks.size()) {
            QModelIndex newModelIndex = createIndex(currentIndex, 0);
            emit dataChanged(newModelIndex, newModelIndex, {CurrentTrackRole});
        }
    }
}

const QList<std::shared_ptr<Track>>& QueueModel::getTrackList() const {
    return tracks;
} 
