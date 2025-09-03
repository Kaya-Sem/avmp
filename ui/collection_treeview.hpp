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
    setModel(PlayerContext::instance()->library()->getModel());
    setHeaderHidden(true);
    setMinimumWidth(250);
    setMaximumWidth(700);
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

          // - No parent = Artist (top level)
          // - Parent has no parent = Album
          // - Parent's parent exists = Track
          QStandardItem *parent = item->parent();
          bool isTrack = parent && parent->parent();

          if (isTrack) {
            TrackItem *trackItem = static_cast<TrackItem *>(item);
            QString filePath = trackItem->getFilePath();
            if (!filePath.isEmpty()) {
              std::shared_ptr<Track> track = std::make_shared<Track>(filePath);
              queue->playNow(track);
            }
          }
        });
  }
};

#endif // !COLLECTION_TREEVIEW
