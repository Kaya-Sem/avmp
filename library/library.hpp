#ifndef LIBRARY
#define LIBRARY

#include "library/album.hpp"
#include "library/artist.hpp"
#include "qtmetamacros.h"
#include "track/track.hpp"
#include <QObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <memory>

class Library : public QObject {
  Q_OBJECT

public:
  explicit Library(QObject *parent = nullptr);

  void addTrack(std::shared_ptr<Track> track);
  QStandardItem *addArtist(std::shared_ptr<Artist> artist);
  QStandardItem *addAlbum(QStandardItem *artistItem,
                          std::shared_ptr<Album> album);
  void addTrackToAlbum(QStandardItem *albumItem, std::shared_ptr<Track> track);

  QStandardItemModel *getModel() const;

  void clear();

private:
  QStandardItemModel *model;

  QStandardItem *findOrCreateArtist(const std::string &artistName);
  QStandardItem *findOrCreateAlbum(QStandardItem *artistItem,
                                   const std::string &albumName);
};

#endif // !LIBRARY
