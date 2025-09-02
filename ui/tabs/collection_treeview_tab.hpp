#ifndef COLLECTION_TREEVIEW_TAB
#define COLLECTION_TREEVIEW_TAB

#include "context/player_context.hpp"
#include "library/library_model.hpp"
#include "qboxlayout.h"
#include "qobject.h"
#include "qtreeview.h"
#include "qwidget.h"
#include "track/track.hpp"
class CollectionTreeviewTab : public QWidget {

public:
  CollectionTreeviewTab(QWidget *parent = nullptr) : QWidget(parent) {

    Queue *queue = PlayerContext::instance()->queue();

    QVBoxLayout *layout = new QVBoxLayout();

    QTreeView *collectionTreeView = new QTreeView();
    collectionTreeView->setModel(
        PlayerContext::instance()->library()->getModel());
    collectionTreeView->setHeaderHidden(true);
    collectionTreeView->setMinimumWidth(250);
    collectionTreeView->setMaximumWidth(700);

    QObject::connect(
        collectionTreeView, &QTreeView::doubleClicked,
        [collectionTreeView, queue](const QModelIndex &index) {
          QStandardItemModel *model =
              qobject_cast<QStandardItemModel *>(collectionTreeView->model());
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

    layout->addWidget(collectionTreeView);
    setLayout(layout);
  }
};

#endif // !COLLECTION_TREEVIEW_TAB
