#ifndef ALBUM
#define ALBUM

#include <string>
class Album {
public:
  explicit Album(std::string name) { this->name = name; }

  std::string name;
};

#endif // !ALBUM
