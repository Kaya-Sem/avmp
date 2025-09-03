#ifndef COLLECTION_TREEVIEW_TAB
#define COLLECTION_TREEVIEW_TAB

#include "../collection_treeview.hpp"
#include "qboxlayout.h"
#include "qwidget.h"

class CollectionTreeviewTab : public QWidget {

public:
  CollectionTreeviewTab(QWidget *parent = nullptr) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout();
    
    CollectionTreeView *collectionTreeView = new CollectionTreeView();
    layout->addWidget(collectionTreeView);
    setLayout(layout);
  }
};

#endif // !COLLECTION_TREEVIEW_TAB
