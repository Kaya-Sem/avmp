#ifndef QUEUE_LISTVIEW
#define QUEUE_LISTVIEW

#include "context/player_context.hpp"
#include "qlistview.h"
#include "qobject.h"

class QueueListView : public QListView {
  Q_OBJECT

public:
  explicit QueueListView(QWidget *parent = nullptr) : QListView(parent) {
    setupListView();
    connectSignals();
  }

private:
  void setupListView() {
    setModel(PlayerContext::instance()->queue()->getModel());
    setSelectionMode(QAbstractItemView::SingleSelection);
    setFrameStyle(QFrame::NoFrame);
  }

  void connectSignals() {
    Queue *queue = PlayerContext::instance()->queue();

    // Connect double-click to play track at that index
    QObject::connect(
        this, &QListView::doubleClicked,
        [this, queue](const QModelIndex &index) {
          if (index.isValid()) {
            queue->playAt(index.row());
          }
        });

    // Connect currentIndexChanged to update selection
    QObject::connect(
        queue, &Queue::currentIndexChanged,
        [this](int newIndex) {
          if (newIndex >= 0) {
            QModelIndex modelIndex = model()->index(newIndex, 0);
            if (modelIndex.isValid()) {
              setCurrentIndex(modelIndex);
              scrollTo(modelIndex, QAbstractItemView::EnsureVisible);
            }
          } else {
            clearSelection();
          }
        });
  }
};

#endif // !QUEUE_LISTVIEW
