#include "library.hpp"
#include "library/library_model.hpp"
#include <QDebug>
#include <QVariant>

Library::Library(QObject *parent) : QObject(parent) {
  treeModel = new QStandardItemModel(this);
}

void Library::addTrack(std::shared_ptr<Track> track) {
  if (!track)
    return;

  QStandardItem *artistItem = findOrCreateArtist(track->artist);
  QStandardItem *albumItem = findOrCreateAlbum(artistItem, track->album);

  // Update the album's year if this track has a valid year and the album doesn't
  AlbumItem *albumItemPtr = static_cast<AlbumItem *>(albumItem);
  if (albumItemPtr && albumItemPtr->getAlbum()) {
    if (albumItemPtr->getAlbum()->getYear() == 0 && track->year > 0) {
      albumItemPtr->getAlbum()->setYear(track->year);
      // Update the display text to show the year
      albumItemPtr->updateDisplayText();
    }
  }

  addTrackToAlbum(albumItem, track);
}

QStandardItem *Library::addArtist(std::shared_ptr<Artist> artist) {
  if (!artist)
    return nullptr;

  ArtistItem *item = new ArtistItem(artist);
  treeModel->appendRow(item);
  return item;
}

QStandardItem *Library::addAlbum(QStandardItem *artistItem,
                                 std::shared_ptr<Album> album) {
  if (!artistItem || !album)
    return nullptr;

  // We weten artistItem is een ArtistItem since it's at the top level
  ArtistItem *artItem = static_cast<ArtistItem *>(artistItem);
  std::shared_ptr<Artist> artist = artItem->getArtist();

  AlbumItem *item = new AlbumItem(album, artist);
  artistItem->appendRow(item);
  return item;
}

void Library::addTrackToAlbum(QStandardItem *albumItem,
                              std::shared_ptr<Track> track) {
  if (!albumItem || !track)
    return;

  TrackItem *trackItem = new TrackItem(track);
  albumItem->appendRow(trackItem);
  
  // If this is the first track added to the album, update the album artwork
  if (albumItem->rowCount() == 1) {
    AlbumItem *albumItemPtr = static_cast<AlbumItem *>(albumItem);
    if (albumItemPtr) {
      albumItemPtr->updateAlbumArtwork();
    }
  }
}

QStandardItemModel *Library::getTreeModel() const { return treeModel; }

void Library::clear() { treeModel->clear(); }

QStandardItem *Library::findOrCreateArtist(const std::string &artistName) {
  std::string artist = artistName.empty() ? "Unknown Artist" : artistName;
  QString qArtistName = QString::fromStdString(artist);

  // Search through top-level items (all are ArtistItems)
  for (int i = 0; i < treeModel->rowCount(); ++i) {
    QStandardItem *item = treeModel->item(i);
    if (item && item->text() == qArtistName) {
      return item;
    }
  }

  std::shared_ptr<Artist> artistObj = std::make_shared<Artist>(artist);
  return addArtist(artistObj);
}

QStandardItem *Library::findOrCreateAlbum(QStandardItem *artistItem,
                                          const std::string &albumName) {
  if (!artistItem)
    return nullptr;

  std::string album = albumName.empty() ? "Unknown Album" : albumName;
  QString qAlbumName = QString::fromStdString(album);

  // Search through children of artist (all are AlbumItems)
  for (int i = 0; i < artistItem->rowCount(); ++i) {
    QStandardItem *item = artistItem->child(i);
    if (item) {
      AlbumItem *albumItem = static_cast<AlbumItem *>(item);
      if (albumItem && albumItem->getAlbum()) {
        QString existingAlbumName = QString::fromStdString(albumItem->getAlbum()->getName());
        if (existingAlbumName == qAlbumName) {
          return item;
        }
      }
    }
  }

  std::shared_ptr<Album> albumObj = std::make_shared<Album>(album);
  return addAlbum(artistItem, albumObj);
}
