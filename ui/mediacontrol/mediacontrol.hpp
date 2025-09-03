#ifndef MEDIACONTROL
#define MEDIACONTROL

#include "context/player_context.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

// MediaControls defines the GUI component responsible for displaying playback controls and the seek indicator
class MediaControls : public QWidget {
  Q_OBJECT

public:
  explicit MediaControls(QWidget *parent = nullptr) : QWidget(parent) {
    setupUI();
    connectSignals();
  }

private slots:
  void onPositionChanged(qint64 position);
  void onDurationChanged(qint64 duration);
  void onSliderMoved(int position);

private:
  void setupUI();
  void connectSignals();
  
  QSlider *positionSlider;
  QLabel *timeLabel;
  QPushButton *playButton;
  QPushButton *pauseButton;
  QPushButton *stopButton;
  QPushButton *previousButton;
  QPushButton *nextButton;
  
  Queue *queue;
  bool isSliderBeingDragged = false;
};

#endif // !MEDIACONTROL
