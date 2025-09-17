#ifndef ALBUM
#define ALBUM

// #include "library/artist.hpp"
#include "track/track.hpp"
#include <memory>
#include <string>
#include <vector>
#include <QPixmap>
#include <QIcon>

class Album {
public:
  explicit Album(std::string name, int year = 0) {
    this->name = name;
    this->year = year;
  }

  std::string const getName() const { return name; }
  int getYear() const { return year; }
  void setYear(int newYear) { year = newYear; }
  std::vector<std::shared_ptr<Track>> getTracks() const { return tracks; }
  
  // Album cover functionality
  void setCoverImage(const QPixmap& cover) { coverImage = cover; }
  QPixmap getCoverImage() const { return coverImage; }
  QIcon getCoverIcon() const { return QIcon(coverImage); }
  bool hasCover() const { return !coverImage.isNull(); }

private:
  std::string name;
  int year;
  QPixmap coverImage;

  const std::vector<std::shared_ptr<Track>> tracks;

  // Cannot include artist yet because of circular dependency
//  const std::shared_ptr<Artist> artist;
};

#endif // !ALBUM
