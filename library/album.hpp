#ifndef ALBUM
#define ALBUM

// #include "library/artist.hpp"
#include "track/track.hpp"
#include <memory>
#include <string>
#include <vector>

class Album {
public:
  explicit Album(std::string name, int year) {
    this->name = name;
    this->year = year;
  }

  std::string getName() const;
  std::vector<std::shared_ptr<Track>> getTracks() const;

private:
  std::string name;
  int year;

  const std::vector<std::shared_ptr<Track>> tracks;


  // Cannot include artist yet because of circular dependency
//  const std::shared_ptr<Artist> artist;
};

#endif // !ALBUM
