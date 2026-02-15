#include "controller.hpp"
#include "track/track.hpp"
#include <QCoreApplication>
#include <QObject>
#include <QtConcurrent>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4item.h>
#include <taglib/vorbisfile.h>
#include <taglib/oggfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/textidentificationframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <vector>

namespace fs = std::filesystem;

Controller::Controller(Library *library, QObject *parent) : QObject(parent) {
  this->library = library;
  libraryPaths.append("/home/kayasem/Music");

  connect(this, &Controller::scanLibraryFinished, this, [this](const std::vector<std::shared_ptr<Track>> &tracks) {
    this->library->clear();
    for (const auto &track : tracks) {
      this->library->addTrack(track);
    }
    emit scanLibraryUpdate("Library scan completed. Loaded " + std::to_string(tracks.size()) + " files");
  });
}

void Controller::setLibraryPaths(const QStringList &paths) {
  libraryPaths = paths;
}

// Helper to get a string from ID3v2 text frame
static std::string getID3v2Text(TagLib::ID3v2::Tag *id3v2, const char *frameId) {
  auto frames = id3v2->frameListMap()[frameId];
  if (!frames.isEmpty()) {
    return frames.front()->toString().to8Bit(true);
  }
  return {};
}

static void extractExtendedTags(TagLib::FileRef &file, std::shared_ptr<Track> &track) {
  std::string path = track->fullPath;
  std::string ext = fs::path(path).extension().string();

  if (ext == ".mp3") {
    auto *mpegFile = dynamic_cast<TagLib::MPEG::File *>(file.file());
    if (mpegFile && mpegFile->ID3v2Tag()) {
      auto *id3v2 = mpegFile->ID3v2Tag();
      track->albumArtist = getID3v2Text(id3v2, "TPE2");
      track->composer = getID3v2Text(id3v2, "TCOM");
      track->codec = "MP3";

      // BPM
      std::string bpmStr = getID3v2Text(id3v2, "TBPM");
      if (!bpmStr.empty()) track->bpm = std::atoi(bpmStr.c_str());

      // Disc number
      std::string discStr = getID3v2Text(id3v2, "TPOS");
      if (!discStr.empty()) {
        // TPOS can be "1/2" format
        track->discNumber = std::atoi(discStr.c_str());
      }

      // Compilation
      std::string tcmp = getID3v2Text(id3v2, "TCMP");
      if (!tcmp.empty()) track->compilation = (tcmp == "1");

      // Lyrics
      auto lyricsFrames = id3v2->frameListMap()["USLT"];
      if (!lyricsFrames.isEmpty()) {
        auto *lf = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame *>(lyricsFrames.front());
        if (lf) track->lyrics = lf->text().to8Bit(true);
      }
    }
  } else if (ext == ".flac") {
    auto *flacFile = dynamic_cast<TagLib::FLAC::File *>(file.file());
    if (flacFile && flacFile->xiphComment()) {
      auto *xiph = flacFile->xiphComment();
      track->codec = "FLAC";

      auto field = [&](const char *key) -> std::string {
        auto list = xiph->fieldListMap()[key];
        if (!list.isEmpty()) return list.front().to8Bit(true);
        return {};
      };

      track->albumArtist = field("ALBUMARTIST");
      track->composer = field("COMPOSER");
      track->lyrics = field("LYRICS");

      std::string bpmStr = field("BPM");
      if (!bpmStr.empty()) track->bpm = std::atoi(bpmStr.c_str());

      std::string discStr = field("DISCNUMBER");
      if (!discStr.empty()) track->discNumber = std::atoi(discStr.c_str());

      std::string compStr = field("COMPILATION");
      if (!compStr.empty()) track->compilation = (compStr == "1");
    }
  } else if (ext == ".ogg") {
    auto *oggFile = dynamic_cast<TagLib::Ogg::Vorbis::File *>(file.file());
    if (oggFile && oggFile->tag()) {
      auto *xiph = oggFile->tag();
      track->codec = "Vorbis";

      auto field = [&](const char *key) -> std::string {
        auto list = xiph->fieldListMap()[key];
        if (!list.isEmpty()) return list.front().to8Bit(true);
        return {};
      };

      track->albumArtist = field("ALBUMARTIST");
      track->composer = field("COMPOSER");
      track->lyrics = field("LYRICS");

      std::string bpmStr = field("BPM");
      if (!bpmStr.empty()) track->bpm = std::atoi(bpmStr.c_str());

      std::string discStr = field("DISCNUMBER");
      if (!discStr.empty()) track->discNumber = std::atoi(discStr.c_str());

      std::string compStr = field("COMPILATION");
      if (!compStr.empty()) track->compilation = (compStr == "1");
    }
  } else if (ext == ".m4a") {
    auto *mp4File = dynamic_cast<TagLib::MP4::File *>(file.file());
    if (mp4File && mp4File->tag()) {
      auto *mp4Tag = mp4File->tag();
      track->codec = "AAC";

      auto items = mp4Tag->itemMap();

      auto getStr = [&](const char *key) -> std::string {
        if (items.contains(key)) {
          return items[key].toStringList().toString().to8Bit(true);
        }
        return {};
      };

      track->albumArtist = getStr("aART");
      track->composer = getStr("\251wrt");
      track->lyrics = getStr("\251lyr");

      if (items.contains("tmpo")) {
        track->bpm = items["tmpo"].toInt();
      }
      if (items.contains("disk")) {
        track->discNumber = items["disk"].toIntPair().first;
      }
      if (items.contains("cpil")) {
        track->compilation = items["cpil"].toBool();
      }
    }
  } else if (ext == ".wav") {
    track->codec = "WAV";
  }
}

void Controller::scanLibrary() {

  emit scanLibraryUpdate("Starting full library (re)scan");
  QCoreApplication::processEvents();

  library->clear();

  std::vector<fs::path> music_files;
  const std::vector<std::string> extentions = {".mp3", ".flac", ".wav", ".ogg",
                                               ".m4a"};

  // Scan all configured library paths
  for (const QString &libraryPath : libraryPaths) {
    try {
      for (const auto &entry : fs::recursive_directory_iterator(libraryPath.toStdString())) {
        if (entry.is_regular_file()) {
          std::string ext = entry.path().extension().string();
          for (const auto &music_ext : extentions) {
            if (ext == music_ext) {
              music_files.push_back(entry.path());
              break;
            }
          }
        }
      }
    } catch (const fs::filesystem_error &e) {
      emit scanLibraryUpdate("Error scanning path: " + libraryPath.toStdString() +
                           " - " + e.what());
    }
  }

  emit scanLibraryUpdate("Found " + std::to_string(music_files.size()) +
                         " music files");

  QCoreApplication::processEvents();

  int processed = 0;
  for (const auto &path : music_files) {

    std::shared_ptr<Track> track_ptr = std::make_shared<Track>(path);

    TagLib::FileRef file(path.c_str());

    if (!file.isNull() && file.tag()) {
      TagLib::Tag *tag = file.tag();

      track_ptr->artist = tag->artist().to8Bit(true);
      track_ptr->album = tag->album().to8Bit(true);
      track_ptr->title = tag->title().to8Bit(true);
      track_ptr->year = tag->year();
      track_ptr->track = tag->track();
      track_ptr->genre = tag->genre().to8Bit(true);
      track_ptr->comment = tag->comment().to8Bit(true);

      // Audio properties
      if (file.audioProperties()) {
        auto *props = file.audioProperties();
        track_ptr->bitrate = props->bitrate();
        track_ptr->sampleRate = props->sampleRate();
        track_ptr->channels = props->channels();
        track_ptr->duration = props->lengthInSeconds();
      }

      // Extended tags (format-specific)
      extractExtendedTags(file, track_ptr);
    } else {
      std::cout << "Failed to read tags from: " << path << "\n";
    }

    library->addTrack(track_ptr);

    processed++;

    if (processed % 50 == 0) {
      emit scanLibraryUpdate("Processing... " + std::to_string(processed) +
                           " of " + std::to_string(music_files.size()) + " files");
      QCoreApplication::processEvents();
    }
  }

  emit scanLibraryUpdate("Library scan completed. Processed " +
                         std::to_string(music_files.size()) + " files");
}

void Controller::scanLibraryAsync() {
  emit scanLibraryUpdate("Starting library scan (background)...");

  QtConcurrent::run([this]() {
    std::vector<std::shared_ptr<Track>> tracks;
    const std::vector<std::string> extentions = {".mp3", ".flac", ".wav", ".ogg", ".m4a"};

    std::vector<fs::path> music_files;

    for (const QString &libraryPath : libraryPaths) {
      try {
        for (const auto &entry : fs::recursive_directory_iterator(libraryPath.toStdString())) {
          if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            for (const auto &music_ext : extentions) {
              if (ext == music_ext) {
                music_files.push_back(entry.path());
                break;
              }
            }
          }
        }
      } catch (const fs::filesystem_error &e) {
        // Can't emit safely from background thread to update UI mid-scan
        std::cerr << "Error scanning path: " << libraryPath.toStdString() << " - " << e.what() << "\n";
      }
    }

    for (const auto &path : music_files) {
      std::shared_ptr<Track> track_ptr = std::make_shared<Track>(path);
      TagLib::FileRef file(path.c_str());

      if (!file.isNull() && file.tag()) {
        TagLib::Tag *tag = file.tag();
        track_ptr->artist = tag->artist().to8Bit(true);
        track_ptr->album = tag->album().to8Bit(true);
        track_ptr->title = tag->title().to8Bit(true);
        track_ptr->year = tag->year();
        track_ptr->track = tag->track();
        track_ptr->genre = tag->genre().to8Bit(true);
        track_ptr->comment = tag->comment().to8Bit(true);

        if (file.audioProperties()) {
          auto *props = file.audioProperties();
          track_ptr->bitrate = props->bitrate();
          track_ptr->sampleRate = props->sampleRate();
          track_ptr->channels = props->channels();
          track_ptr->duration = props->lengthInSeconds();
        }

        extractExtendedTags(file, track_ptr);
      }

      tracks.push_back(track_ptr);
    }

    emit scanLibraryFinished(tracks);
  });
}
