#ifndef COLLECTION_TREEVIEW
#define COLLECTION_TREEVIEW

#include "context/player_context.hpp"
#include "library/library_model.hpp"
#include "qobject.h"
#include "qtreeview.h"
#include "track/track.hpp"

class CollectionTreeView : public QTreeView {
  Q_OBJECT

public:
  explicit CollectionTreeView(QWidget *parent = nullptr) : QTreeView(parent) {
    setupTreeView();
    connectSignals();
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

          // Cast to LibraryItem to use the abstract base class
          LibraryItem *libraryItem = static_cast<LibraryItem *>(item);
          if (!libraryItem)
            return;

          // Use getType() to determine the item type and handle accordingly
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
};

#endif // !COLLECTION_TREEVIEW
