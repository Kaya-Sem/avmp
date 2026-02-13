#ifndef PLAYLIST
#define PLAYLIST

#include "track/track.hpp"
#include <filesystem>
#include <memory>
#include <vector>
#include <QString>

namespace fs = std::filesystem;

class Playlist {

public:
  explicit Playlist(const std::string& name);

  static Playlist load(const fs::path& playlistDir);
  static fs::path playlistsBaseDir();

  void save();
  void addTrack(const Track& track);
  void removeTrack(int index);

  QString getName() const;
  std::vector<std::shared_ptr<Track>>& getTracks();
  fs::path getPlaylistDir() const;

private:
  std::string name;
  fs::path playlistDir;
  std::vector<std::shared_ptr<Track>> tracks;

  std::string makeHardlinkFilename(int index, const Track& track) const;
  void renumberHardlinks();
};

#endif // !PLAYLIST
