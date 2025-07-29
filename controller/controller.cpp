#include "controller.hpp"
#include <QObject>
#include <filesystem>
#include <iostream>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <vector>

namespace fs = std::filesystem;

Controller::Controller(Library *library, QObject *parent) : QObject(parent) {
  this->library = library;
}

void Controller::scanLibrary() {

  emit scanLibraryUpdate("Starting library scan...");

  std::vector<fs::path> music_files;
  const std::vector<std::string> extentions = {".mp3", ".flac", ".wav", ".ogg",
                                               ".m4a"};

  for (const auto &entry :
       fs::recursive_directory_iterator("/home/kayasem/Music/Library")) {
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

  for (const auto &path : music_files) {
    TagLib::FileRef file(path.c_str());
    if (!file.isNull() && file.tag()) {
      TagLib::Tag *tag = file.tag();
      std::string title = tag->title().to8Bit(true);
      std::string artist = tag->artist().to8Bit(true);
      std::string album = tag->album().to8Bit(true);
      std::string genre = tag->genre().to8Bit(true);

      std::cout << "File: " << path << "\n"
                << "  Title : " << title << "\n"
                << "  Artist: " << artist << "\n"
                << "  Genre: " << genre << "\n"
                << "  Album : " << album << "\n\n";

    } else {
      std::cout << "Failed to read tags from: " << path << "\n";
    }
  }

  std::cout << music_files.size() << "\n";

  emit scanLibraryUpdate("Library scan completed. Processed " +
                         std::to_string(music_files.size()) + " files");
}
