#ifndef LIBRARY_MODEL
#define LIBRARY_MODEL

#include "library/album.hpp"
#include "library/artist.hpp"
#include "qobject.h"
#include "qstandarditemmodel.h"
#include "track/track.hpp"
#include <memory>

class ArtistItem : public QStandardItem {
public:
  explicit ArtistItem(std::shared_ptr<Artist> artist)
      : QStandardItem(
            QString::fromStdString(artist ? artist->name : "Unknown Artist")),
        artistPtr(artist) {
  }

  std::shared_ptr<Artist> getArtist() const { return artistPtr; }
  const QString getArtistName() const {
    return artistPtr ? QString::fromStdString(artistPtr->name)
                     : "Unknown Artist";
  }

private:
  std::shared_ptr<Artist> artistPtr;
};

class AlbumItem : public QStandardItem {
public:
  explicit AlbumItem(std::shared_ptr<Album> album,
                     std::shared_ptr<Artist> artist)
      : QStandardItem(
            QString::fromStdString(album ? album->getName() : "Unknown Album")),
        albumPtr(album), artistPtr(artist) {
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

class TrackItem : public QStandardItem {
public:
  explicit TrackItem(std::shared_ptr<Track> track)
      : QStandardItem(getDisplayTitle(track)), trackPtr(track) {
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
