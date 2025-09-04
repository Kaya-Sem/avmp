#ifndef QUEUE_MODEL_HPP
#define QUEUE_MODEL_HPP

#include <QAbstractListModel>
#include <QList>
#include <memory>
#include "track/track.hpp"

class QueueModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum QueueRoles {
        DisplayRole = Qt::DisplayRole,
        CurrentTrackRole = Qt::UserRole + 1
    };
    
    explicit QueueModel(QObject *parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    std::shared_ptr<Track> getTrack(int index) const;
    
    // Queue management functions
    void appendTrack(std::shared_ptr<Track> track);
    void insertTrack(std::shared_ptr<Track> track, int index);
    void removeTrack(int index);
    void clear();
    
    // Get the underlying track list
    const QList<std::shared_ptr<Track>>& getTrackList() const;
    
    // Current track management
    void setCurrentIndex(int index);
    int getCurrentIndex() const { return currentIndex; }
    
private:
    QList<std::shared_ptr<Track>> tracks;
    int currentIndex = -1;
};

#endif // QUEUE_MODEL_HPP 
