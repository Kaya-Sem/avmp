#ifndef LIBRARY_MODEL
#define LIBRARY_MODEL

#include "library/album.hpp"
#include "library/artist.hpp"
#include "qobject.h"
#include "qstandarditemmodel.h"
#include "track/track.hpp"
#include <memory>
#include <QList>

class LibraryItem : public QStandardItem {
public:
  enum class Type {
    Artist,
    Album,
    Track
  };

  explicit LibraryItem(const QString &text) : QStandardItem(text) {}
  virtual ~LibraryItem() = default;
  
  virtual Type getType() const = 0;
  
  // Helper method to collect all tracks from this item and its children
  virtual QList<std::shared_ptr<Track>> getAllTracks() const = 0;
};

class ArtistItem : public LibraryItem {
public:
  explicit ArtistItem(std::shared_ptr<Artist> artist)
      : LibraryItem(
            QString::fromStdString(artist ? artist->name : "Unknown Artist")),
        artistPtr(artist) {
  }

  Type getType() const override { return Type::Artist; }
  QList<std::shared_ptr<Track>> getAllTracks() const override {
    QList<std::shared_ptr<Track>> tracks;
    
    // Iterate through all albums (children of this artist)
    for (int i = 0; i < rowCount(); ++i) {
      QStandardItem *albumItem = child(i);
      if (albumItem) {
        LibraryItem *libraryAlbumItem = dynamic_cast<LibraryItem *>(albumItem);
        if (libraryAlbumItem) {
          tracks.append(libraryAlbumItem->getAllTracks());
        }
      }
    }
    
    return tracks;
  }
  std::shared_ptr<Artist> getArtist() const { return artistPtr; }
  const QString getArtistName() const {
    return artistPtr ? QString::fromStdString(artistPtr->name)
                     : "Unknown Artist";
  }

private:
  std::shared_ptr<Artist> artistPtr;
};

class AlbumItem : public LibraryItem {
public:
  explicit AlbumItem(std::shared_ptr<Album> album,
                     std::shared_ptr<Artist> artist)
      : LibraryItem(
            QString::fromStdString(album ? album->getName() : "Unknown Album")),
        albumPtr(album), artistPtr(artist) {
  }

  Type getType() const override { return Type::Album; }
  QList<std::shared_ptr<Track>> getAllTracks() const override {
    QList<std::shared_ptr<Track>> tracks;
    
    // Iterate through all tracks (children of this album)
    for (int i = 0; i < rowCount(); ++i) {
      QStandardItem *trackItem = child(i);
      if (trackItem) {
        LibraryItem *libraryTrackItem = dynamic_cast<LibraryItem *>(trackItem);
        if (libraryTrackItem && libraryTrackItem->getType() == Type::Track) {
          // Use the virtual method instead of casting to specific type
          QList<std::shared_ptr<Track>> childTracks = libraryTrackItem->getAllTracks();
          tracks.append(childTracks);
        }
      }
    }
    
    return tracks;
  }
  std::shared_ptr<Album> getAlbum() const { return albumPtr; }
  std::shared_ptr<Artist> getArtist() const { return artistPtr; }
  const QString getAlbumName() const {
    return albumPtr ? QString::fromStdString(albumPtr->getName())
                    : "Unknown Album";
  }
  const QString getArtistName() const {
    return artistPtr ? QString::fromStdString(artistPtr->name)
                     : "Unknown Artist";
  }

private:
  std::shared_ptr<Album> albumPtr;
  std::shared_ptr<Artist> artistPtr;
};

class TrackItem : public LibraryItem {
public:
  explicit TrackItem(std::shared_ptr<Track> track)
      : LibraryItem(getDisplayTitle(track)), trackPtr(track) {
  }

  Type getType() const override { return Type::Track; }
  QList<std::shared_ptr<Track>> getAllTracks() const override {
    QList<std::shared_ptr<Track>> tracks;
    if (trackPtr) {
      tracks.append(trackPtr);
    }
    return tracks;
  }
  std::shared_ptr<Track> getTrack() const { return trackPtr; }
  const QString getFilePath() const {
    return trackPtr ? QString::fromStdString(trackPtr->fullPath) : "";
  }
  const QString getArtist() const {
    return trackPtr ? QString::fromStdString(trackPtr->artist)
                    : "Unknown Artist";
  }
  const QString getAlbum() const {
    return trackPtr ? QString::fromStdString(trackPtr->album) : "Unknown Album";
  }
  const QString getTitle() const {
    return trackPtr ? QString::fromStdString(trackPtr->title) : "Unknown Track";
  }

private:
  std::shared_ptr<Track> trackPtr;

  static QString getDisplayTitle(std::shared_ptr<Track> track) {
    if (!track || track->title.empty())
      return "Unknown Track";

    return QString::fromStdString(track->title);
  }
};

#endif // !LIBRARY_MODEL
