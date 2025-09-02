#ifndef MEDIACONTROL
#define MEDIACONTROL

#include "context/player_context.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
class MediaControls : public QWidget {

public:
  explicit MediaControls(QWidget *parent = nullptr) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout();

    QPushButton *playButton = new QPushButton("Play");
    QPushButton *pauseButton = new QPushButton("Pause");
    QPushButton *stopButton = new QPushButton("Stop");

    layout->addWidget(playButton);
    layout->addWidget(pauseButton);
    layout->addWidget(stopButton);

    Queue *queue = PlayerContext::instance()->queue();

    QObject::connect(playButton, &QPushButton::clicked,
                     [&queue]() { queue->play(); });

    QObject::connect(stopButton, &QPushButton::clicked,
                     [&queue]() { queue->stop(); });

    QObject::connect(pauseButton, &QPushButton::clicked,
                     [&queue]() { queue->pause(); });

    setLayout(layout);
  };
};

#endif // !MEDIACONTROL
