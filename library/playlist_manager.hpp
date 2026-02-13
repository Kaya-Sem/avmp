#ifndef PLAYLIST_MANAGER
#define PLAYLIST_MANAGER

#include "playlist.hpp"
#include <QObject>
#include <QStandardItemModel>
#include <memory>

class PlaylistManager : public QObject {
  Q_OBJECT

public:
  explicit PlaylistManager(QObject* parent = nullptr);

  void loadAll();
  void createPlaylist(const QString& name);
  void deletePlaylist(const QString& name);
  std::shared_ptr<Playlist> getPlaylist(const QString& name);
  QStringList getPlaylistNames() const;
  QStandardItemModel* getModel();

  const QList<std::shared_ptr<Playlist>>& getPlaylists() const;
  void refreshModel();

signals:
  void playlistsChanged();

private:
  QList<std::shared_ptr<Playlist>> playlists;
  QStandardItemModel* model;
};

#endif // !PLAYLIST_MANAGER
