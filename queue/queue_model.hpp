#ifndef QUEUE_MODEL_HPP
#define QUEUE_MODEL_HPP

#include <QAbstractListModel>
#include <QList>
#include <memory>
#include "track/track.hpp"

class QueueModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit QueueModel(QObject *parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    // Custom functions instead of roles
    QString getTrackTitle(int index) const;
    QString getTrackArtist(int index) const;
    QString getTrackAlbum(int index) const;
    QString getTrackPath(int index) const;
    std::shared_ptr<Track> getTrack(int index) const;
    
    // Queue management functions
    void appendTrack(std::shared_ptr<Track> track);
    void insertTrack(int index, std::shared_ptr<Track> track);
    void removeTrack(int index);
    void clear();
    
    // Get the underlying track list
    const QList<std::shared_ptr<Track>>& getTrackList() const;
    
private:
    QList<std::shared_ptr<Track>> tracks;
};

#endif // QUEUE_MODEL_HPP 