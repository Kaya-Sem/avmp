#include "controller.hpp"
#include "track/track.hpp"
#include <QCoreApplication>
#include <QObject>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <vector>

namespace fs = std::filesystem;

Controller::Controller(Library *library, QObject *parent) : QObject(parent) {
  this->library = library;
}

void Controller::scanLibrary() {

  emit scanLibraryUpdate("Starting full library (re)scan");
  QCoreApplication::processEvents();

  library->clear();

  std::vector<fs::path> music_files;
  const std::vector<std::string> extentions = {".mp3", ".flac", ".wav", ".ogg",
                                               ".m4a"};

  for (const auto &entry :
       fs::recursive_directory_iterator("/home/kayasem/Music")) {
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
