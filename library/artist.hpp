#ifndef ARTIST
#define ARTIST

#include "library/album.hpp"
#include <memory>
#include <string>
#include <vector>

class Artist {

public:
  Artist(std::string name) { this->name = name; }

  std::string name;
  std::vector<std::shared_ptr<Album>> albums;
};

#endif // !ARTIST
