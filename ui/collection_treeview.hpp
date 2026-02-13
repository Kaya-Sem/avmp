#ifndef COLLECTION_TREEVIEW
#define COLLECTION_TREEVIEW

#include "context/player_context.hpp"
#include "library/library_model.hpp"
#include "library/playlist_manager.hpp"
#include "qobject.h"
#include "qtreeview.h"
#include "track/track.hpp"
#include <QInputDialog>
#include <QMenu>

class CollectionTreeView : public QTreeView {
  Q_OBJECT

public:
  explicit CollectionTreeView(QWidget *parent = nullptr) : QTreeView(parent) {
    setupTreeView();
    connectSignals();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested,
            this, &CollectionTreeView::showContextMenu);
  }

private:
  void setupTreeView() {
    setModel(PlayerContext::instance()->library()->getTreeModel());
    setHeaderHidden(true);
    setMinimumWidth(250);
    setMaximumWidth(700);
    setIconSize(QSize(64, 64));
  }

  void connectSignals() {
    Queue *queue = PlayerContext::instance()->queue();

    QObject::connect(
        this, &QTreeView::doubleClicked,
        [this, queue](const QModelIndex &index) {
          QStandardItemModel *model =
              qobject_cast<QStandardItemModel *>(this->model());
          if (!model)
            return;

          QStandardItem *item = model->itemFromIndex(index);
          if (!item)
            return;

          LibraryItem *libraryItem = static_cast<LibraryItem *>(item);
          if (!libraryItem)
            return;

          switch (libraryItem->getType()) {
            case LibraryItem::Type::Track: {
              TrackItem *trackItem = static_cast<TrackItem *>(libraryItem);
              QString filePath = trackItem->getFilePath();
              if (!filePath.isEmpty()) {
                std::shared_ptr<Track> track = std::make_shared<Track>(filePath);
                queue->playNow(track);
              }
              break;
            }
            case LibraryItem::Type::Album: {
              AlbumItem *albumItem = static_cast<AlbumItem *>(libraryItem);
              QList<std::shared_ptr<Track>> tracks = albumItem->getAllTracks();

              if (!tracks.isEmpty()) {
                for (const auto &track : tracks) {
                  queue->appendTrack(track);
                }
              }
              break;
            }
            case LibraryItem::Type::Artist: {
              ArtistItem *artistItem = static_cast<ArtistItem *>(libraryItem);
              QList<std::shared_ptr<Track>> tracks = artistItem->getAllTracks();

              if (!tracks.isEmpty()) {
                for (const auto &track : tracks) {
                  queue->appendTrack(track);
                }
              }
              break;
            }
          }
        });
  }

  void showContextMenu(const QPoint &pos) {
    QModelIndex index = indexAt(pos);
    if (!index.isValid())
      return;

    QStandardItemModel *stdModel =
        qobject_cast<QStandardItemModel *>(model());
    if (!stdModel)
      return;

    QStandardItem *item = stdModel->itemFromIndex(index);
    if (!item)
      return;

    LibraryItem *libraryItem = static_cast<LibraryItem *>(item);
    if (!libraryItem)
      return;

    QList<std::shared_ptr<Track>> tracks = libraryItem->getAllTracks();
    if (tracks.isEmpty())
      return;

    PlaylistManager *pm = PlayerContext::instance()->playlistManager();
    QMenu menu(this);

    QMenu *addToPlaylist = menu.addMenu("Add to Playlist");

    QStringList names = pm->getPlaylistNames();
    for (const QString &name : names) {
      addToPlaylist->addAction(name, [pm, name, tracks]() {
        auto playlist = pm->getPlaylist(name);
        if (playlist) {
          for (const auto &track : tracks) {
            playlist->addTrack(*track);
          }
          pm->refreshModel();
        }
      });
    }

    if (!names.isEmpty()) {
      addToPlaylist->addSeparator();
    }

    addToPlaylist->addAction("New Playlist...", [this, pm, tracks]() {
      bool ok;
      QString name = QInputDialog::getText(this, "New Playlist",
                                           "Playlist name:", QLineEdit::Normal,
                                           "", &ok);
      if (ok && !name.isEmpty()) {
        pm->createPlaylist(name);
        auto playlist = pm->getPlaylist(name);
        if (playlist) {
          for (const auto &track : tracks) {
            playlist->addTrack(*track);
          }
          pm->refreshModel();
        }
      }
    });

    menu.exec(viewport()->mapToGlobal(pos));
  }
};

#endif // !COLLECTION_TREEVIEW
