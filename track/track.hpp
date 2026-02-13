#ifndef TRACK
#define TRACK

#include "qobject.h"
#include <string>

class Track {
public:
  explicit Track(std::string fullPath) { this->fullPath = std::move(fullPath); };

  explicit Track(QString fullPath) : Track(fullPath.toStdString()) {};

  std::string fullPath;
  std::string comment;
  std::string album;
  std::string artist;
  std::string title;
  std::string genre;
  int year = 0;
  int track = 0;

  // Extended metadata
  std::string composer;
  std::string albumArtist;
  std::string lyrics;
  int discNumber = 0;
  int bpm = 0;
  bool compilation = false;

  // Audio properties
  int bitrate = 0;
  int sampleRate = 0;
  int channels = 0;
  int duration = 0;
  std::string codec;
};

#endif // !TRACK
