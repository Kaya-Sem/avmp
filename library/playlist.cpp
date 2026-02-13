#include "playlist.hpp"

#include <QStandardPaths>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <taglib/fileref.h>
#include <taglib/tag.h>

Playlist::Playlist(const std::string& name) : name(name) {
  playlistDir = playlistsBaseDir() / name;
}

fs::path Playlist::playlistsBaseDir() {
  QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
  fs::path base = fs::path(dataDir.toStdString()) / "avmp" / "playlists";
  if (!fs::exists(base)) {
    fs::create_directories(base);
  }
  return base;
}

Playlist Playlist::load(const fs::path& playlistDir) {
  std::string dirName = playlistDir.filename().string();
  Playlist playlist(dirName);

  fs::path m3uPath = playlistDir / (dirName + ".m3u");
  if (!fs::exists(m3uPath)) {
    return playlist;
  }

  std::ifstream file(m3uPath);
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    fs::path trackPath = playlistDir / line;
    if (fs::exists(trackPath)) {
      auto track = std::make_shared<Track>(QString::fromStdString(trackPath.string()));

      TagLib::FileRef f(trackPath.string().c_str());
      if (!f.isNull() && f.tag()) {
        TagLib::Tag* tag = f.tag();
        track->title = tag->title().toCString(true);
        track->artist = tag->artist().toCString(true);
        track->album = tag->album().toCString(true);
        track->genre = tag->genre().toCString(true);
        track->year = tag->year();
        track->track = tag->track();
      }

      playlist.tracks.push_back(track);
    }
  }

  return playlist;
}

void Playlist::save() {
  if (!fs::exists(playlistDir)) {
    fs::create_directories(playlistDir);
  }

  fs::path m3uPath = playlistDir / (name + ".m3u");
  std::ofstream file(m3uPath);
  file << "#EXTM3U\n";

  for (size_t i = 0; i < tracks.size(); ++i) {
    const auto& track = tracks[i];
    std::string filename = fs::path(track->fullPath).filename().string();

    int duration = 0;
    TagLib::FileRef f(track->fullPath.c_str());
    if (!f.isNull() && f.audioProperties()) {
      duration = f.audioProperties()->lengthInSeconds();
    }

    std::string displayTitle = track->artist.empty()
        ? track->title
        : track->artist + " - " + track->title;
    if (displayTitle.empty()) {
      displayTitle = fs::path(track->fullPath).stem().string();
    }

    file << "#EXTINF:" << duration << "," << displayTitle << "\n";
    file << filename << "\n";
  }
}

void Playlist::addTrack(const Track& track) {
  int index = static_cast<int>(tracks.size());
  std::string filename = makeHardlinkFilename(index, track);

  if (!fs::exists(playlistDir)) {
    fs::create_directories(playlistDir);
  }

  fs::path linkPath = playlistDir / filename;
  if (!fs::exists(linkPath)) {
    fs::create_hard_link(track.fullPath, linkPath);
  }

  auto newTrack = std::make_shared<Track>(QString::fromStdString(linkPath.string()));
  newTrack->title = track.title;
  newTrack->artist = track.artist;
  newTrack->album = track.album;
  newTrack->genre = track.genre;
  newTrack->year = track.year;
  newTrack->track = track.track;

  tracks.push_back(newTrack);
  save();
}

void Playlist::removeTrack(int index) {
  if (index < 0 || index >= static_cast<int>(tracks.size())) {
    return;
  }

  // Remove the hardlink file
  std::string filename = makeHardlinkFilename(index, *tracks[index]);
  fs::path linkPath = playlistDir / filename;
  if (fs::exists(linkPath)) {
    fs::remove(linkPath);
  }

  tracks.erase(tracks.begin() + index);
  renumberHardlinks();
  save();
}

QString Playlist::getName() const {
  return QString::fromStdString(name);
}

std::vector<std::shared_ptr<Track>>& Playlist::getTracks() {
  return tracks;
}

fs::path Playlist::getPlaylistDir() const {
  return playlistDir;
}

std::string Playlist::makeHardlinkFilename(int index, const Track& track) const {
  fs::path origPath(track.fullPath);
  std::string stem = origPath.stem().string();
  std::string ext = origPath.extension().string();

  // Sanitize stem: replace problematic characters
  std::string safeStem;
  for (char c : stem) {
    if (c == '/' || c == '\0') {
      safeStem += '_';
    } else {
      safeStem += c;
    }
  }

  char prefix[16];
  snprintf(prefix, sizeof(prefix), "%02d-", index + 1);
  return std::string(prefix) + safeStem + ext;
}

void Playlist::renumberHardlinks() {
  // Rename all hardlinks to match their new indices
  for (size_t i = 0; i < tracks.size(); ++i) {
    fs::path oldPath(tracks[i]->fullPath);
    std::string newFilename = makeHardlinkFilename(static_cast<int>(i), *tracks[i]);
    fs::path newPath = playlistDir / newFilename;

    if (oldPath != newPath && fs::exists(oldPath)) {
      fs::rename(oldPath, newPath);
      tracks[i]->fullPath = newPath.string();
    }
  }
}
