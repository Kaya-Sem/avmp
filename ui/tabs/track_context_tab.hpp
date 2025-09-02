#ifndef TRACK_CONTEXT_TAB
#define TRACK_CONTEXT_TAB

#include <QVBoxLayout>
#include <QWidget>

class TrackContextTab : public QWidget {

public:
  TrackContextTab(QWidget *parent = nullptr) : QWidget(parent) {

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);
  }
};

#endif // TRACK_CONTEXT_TAB
