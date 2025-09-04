#ifndef PLAYLIST
#define PLAYLIST

#include "track/track.hpp"
#include <filesystem>
#include <memory>
#include <vector>

class Playlist {

public:
  explicit Playlist(const std::string name);

private:
  std::filesystem::path coverImagePath;
  std::string name;
  std::vector<std::shared_ptr<Track>> tracks;
};

#endif // !PLAYLIST
