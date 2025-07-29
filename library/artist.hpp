#ifndef ARTIST
#define ARTIST

#include "library/album.hpp"
#include <string>
#include <vector>

class Artist {

public:
  Artist(std::string name) { this->name = name; }
  Artist(std::string name, int year) {
    this->name = name;
    this->year = year;
  }

  std::string name;
  std::vector<Album> albums;
  int year;
};

#endif // !ARTIST
