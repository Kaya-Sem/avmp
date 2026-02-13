#include "playlist_manager.hpp"

#include <QStandardItem>
#include <filesystem>

namespace fs = std::filesystem;

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent), model(new QStandardItemModel(this)) {}

void PlaylistManager::loadAll() {
  playlists.clear();

  fs::path baseDir = Playlist::playlistsBaseDir();
  if (!fs::exists(baseDir)) {
    return;
  }

  for (const auto& entry : fs::directory_iterator(baseDir)) {
    if (entry.is_directory()) {
      auto playlist = std::make_shared<Playlist>(Playlist::load(entry.path()));
      playlists.append(playlist);
    }
  }

  refreshModel();
}

void PlaylistManager::createPlaylist(const QString& name) {
  for (const auto& p : playlists) {
    if (p->getName() == name) {
      return;
    }
  }

  auto playlist = std::make_shared<Playlist>(name.toStdString());
  playlist->save();
  playlists.append(playlist);
  refreshModel();
  emit playlistsChanged();
}

void PlaylistManager::deletePlaylist(const QString& name) {
  for (int i = 0; i < playlists.size(); ++i) {
    if (playlists[i]->getName() == name) {
      fs::path dir = playlists[i]->getPlaylistDir();
      if (fs::exists(dir)) {
        fs::remove_all(dir);
      }
      playlists.removeAt(i);
      refreshModel();
      emit playlistsChanged();
      return;
    }
  }
}

std::shared_ptr<Playlist> PlaylistManager::getPlaylist(const QString& name) {
  for (const auto& p : playlists) {
    if (p->getName() == name) {
      return p;
    }
  }
  return nullptr;
}

QStringList PlaylistManager::getPlaylistNames() const {
  QStringList names;
  for (const auto& p : playlists) {
    names << p->getName();
  }
  return names;
}

QStandardItemModel* PlaylistManager::getModel() {
  return model;
}

const QList<std::shared_ptr<Playlist>>& PlaylistManager::getPlaylists() const {
  return playlists;
}

void PlaylistManager::refreshModel() {
  model->clear();

  for (const auto& playlist : playlists) {
    auto* playlistItem = new QStandardItem(playlist->getName());
    playlistItem->setEditable(false);

    for (const auto& track : playlist->getTracks()) {
      QString display = QString::fromStdString(track->title);
      if (display.isEmpty()) {
        display = QString::fromStdString(
            fs::path(track->fullPath).stem().string());
      }
      if (!track->artist.empty()) {
        display = QString::fromStdString(track->artist) + " - " + display;
      }

      auto* trackItem = new QStandardItem(display);
      trackItem->setEditable(false);
      trackItem->setData(QString::fromStdString(track->fullPath),
                         Qt::UserRole + 1);
      playlistItem->appendRow(trackItem);
    }

    model->appendRow(playlistItem);
  }
}
