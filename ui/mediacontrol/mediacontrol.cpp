#include "mediacontrol.hpp"
#include <QMediaPlayer>
#include <QAudioOutput>

void MediaControls::setupUI() {
  // Set fixed height to prevent taking up too much space
  setFixedHeight(120);
  
  // Create main vertical layout
  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(10, 5, 10, 5);
  mainLayout->setSpacing(5);
  
  // Create position slider
  positionSlider = new QSlider(Qt::Horizontal);
  positionSlider->setRange(0, 1000);
  positionSlider->setValue(0);
  
  // Create time label
  timeLabel = new QLabel("00:00 / 00:00");
  timeLabel->setAlignment(Qt::AlignCenter);
  timeLabel->setMinimumWidth(100);
  
  // Create control buttons
  playButton = new QPushButton("▶");
  pauseButton = new QPushButton("⏸");
  stopButton = new QPushButton("⏹");
  previousButton = new QPushButton("⏮");
  nextButton = new QPushButton("⏭");
  
  // Set button sizes
  int buttonSize = 40;
  playButton->setFixedSize(buttonSize, buttonSize);
  pauseButton->setFixedSize(buttonSize, buttonSize);
  stopButton->setFixedSize(buttonSize, buttonSize);
  previousButton->setFixedSize(buttonSize, buttonSize);
  nextButton->setFixedSize(buttonSize, buttonSize);
  
  // Create horizontal layout for buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(10);
  buttonLayout->addStretch();
  buttonLayout->addWidget(previousButton);
  buttonLayout->addWidget(playButton);
  buttonLayout->addWidget(pauseButton);
  buttonLayout->addWidget(stopButton);
  buttonLayout->addWidget(nextButton);
  buttonLayout->addStretch();

  QHBoxLayout *sliderLayout = new QHBoxLayout();
  sliderLayout->addWidget(positionSlider);
  sliderLayout->addWidget(timeLabel);
  

  mainLayout->addLayout(sliderLayout);
  mainLayout->addLayout(buttonLayout);
  
  setLayout(mainLayout);
}

void MediaControls::connectSignals() {
  queue = PlayerContext::instance()->queue();
  
  // Connect button signals
  QObject::connect(playButton, &QPushButton::clicked,
                   [this]() { queue->play(); });
  
  QObject::connect(pauseButton, &QPushButton::clicked,
                   [this]() { queue->pause(); });
  
  QObject::connect(stopButton, &QPushButton::clicked,
                   [this]() { queue->stop(); });
  
  QObject::connect(previousButton, &QPushButton::clicked,
                   [this]() { queue->previous(); });
  
  QObject::connect(nextButton, &QPushButton::clicked,
                   [this]() { queue->next(); });
  
  // Connect slider signals
  QObject::connect(positionSlider, &QSlider::sliderPressed,
                   [this]() { isSliderBeingDragged = true; });
  
  QObject::connect(positionSlider, &QSlider::sliderReleased,
                   [this]() { isSliderBeingDragged = false; });
  
  QObject::connect(positionSlider, &QSlider::valueChanged,
                   this, &MediaControls::onSliderMoved);
  
  // Connect to media player signals
  QMediaPlayer *player = queue->getPlayer();
  if (player) {
    QObject::connect(player, &QMediaPlayer::positionChanged,
                     this, &MediaControls::onPositionChanged);
    
    QObject::connect(player, &QMediaPlayer::durationChanged,
                     this, &MediaControls::onDurationChanged);
  }
}

void MediaControls::onPositionChanged(qint64 position) {
  if (!isSliderBeingDragged) {
    QMediaPlayer *player = queue->getPlayer();
    if (player && player->duration() > 0) {
      int sliderValue = static_cast<int>((position * 1000) / player->duration());
      positionSlider->setValue(sliderValue);
      
      // Update time label
      int currentSeconds = static_cast<int>(position / 1000);
      int totalSeconds = static_cast<int>(player->duration() / 1000);
      
      QString timeText = QString("%1:%2 / %3:%4")
          .arg(currentSeconds / 60, 2, 10, QChar('0'))
          .arg(currentSeconds % 60, 2, 10, QChar('0'))
          .arg(totalSeconds / 60, 2, 10, QChar('0'))
          .arg(totalSeconds % 60, 2, 10, QChar('0'));
      
      timeLabel->setText(timeText);
    }
  }
}

void MediaControls::onDurationChanged(qint64 duration) {
  // Duration changed, update slider range if needed
  // The slider range is already set to 0-1000, so we just need to update the time label
  if (duration > 0) {
    int totalSeconds = static_cast<int>(duration / 1000);
    QString timeText = QString("00:00 / %1:%2")
        .arg(totalSeconds / 60, 2, 10, QChar('0'))
        .arg(totalSeconds % 60, 2, 10, QChar('0'));
    timeLabel->setText(timeText);
  }
}

void MediaControls::onSliderMoved(int position) {
  if (isSliderBeingDragged) {
    QMediaPlayer *player = queue->getPlayer();
    if (player && player->duration() > 0) {
      qint64 newPosition = (position * player->duration()) / 1000;
      player->setPosition(newPosition);
    }
  }
}
