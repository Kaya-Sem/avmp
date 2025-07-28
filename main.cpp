
#include "qframe.h"
#include "qlabel.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QFileDialog>
#include <QDir>
#include <QPushButton>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMediaPlayer *player = new QMediaPlayer();
  QAudioOutput *audioOutput = new QAudioOutput();
  player->setAudioOutput(audioOutput);
  
  audioOutput->setVolume(0.5);

  QMainWindow window;
  window.setWindowTitle("ssmp");

  QWidget *centralWidget = new QWidget;
  window.setCentralWidget(centralWidget);

  QVBoxLayout *vbox = new QVBoxLayout;
  QHBoxLayout *hbox = new QHBoxLayout();

  hbox->addWidget(new QLabel("Sidebar"));
  hbox->addWidget(new QLabel("Main window"));

  QFrame *mediacontrol_frame = new QFrame();

  QHBoxLayout *mediacontrols_hbox = new QHBoxLayout();
  mediacontrol_frame->setLayout(mediacontrols_hbox);

  mediacontrol_frame->setObjectName("mediacontrol");
  mediacontrol_frame->setStyleSheet(
      "#mediacontrol { border: 2px solid black; }");

  QPushButton *playButton = new QPushButton("Play");
  QPushButton *pauseButton = new QPushButton("Pause");
  QPushButton *stopButton = new QPushButton("Stop");
  
  mediacontrols_hbox->addWidget(playButton);
  mediacontrols_hbox->addWidget(pauseButton);
  mediacontrols_hbox->addWidget(stopButton);

  vbox->addLayout(hbox);
  vbox->addWidget(mediacontrol_frame);

  centralWidget->setLayout(vbox);

  // Menu bar
  QMenuBar *menuBar = window.menuBar();
  
  // File menu
  QMenu *fileMenu = menuBar->addMenu("&File");
  QAction *newAction = fileMenu->addAction("&New");
  QAction *openAction = fileMenu->addAction("&Open");
  QAction *saveAction = fileMenu->addAction("&Save");
  fileMenu->addSeparator();
  QAction *exitAction = fileMenu->addAction("E&xit");
  
  // Edit menu
  QMenu *editMenu = menuBar->addMenu("&Library");
  QAction *undoAction = editMenu->addAction("&Full scan");
  editMenu->addSeparator();
  QAction *cutAction = editMenu->addAction("&Play song");
  
  // View menu
  QMenu *viewMenu = menuBar->addMenu("&View");
  QAction *fullscreenAction = viewMenu->addAction("&Fullscreen");
  QAction *minimizeAction = viewMenu->addAction("&Minimize");
  

 QObject::connect(cutAction, &QAction::triggered, [&window, player]() {
    // For now, use a hardcoded path (update this to your music file)
    QString musicFile = "/home/kayasem/Music/sample.mp3";  // Update this path
    player->setSource(QUrl::fromLocalFile(musicFile));
    player->play();
  });

  QObject::connect(openAction, &QAction::triggered, [&window, player]() {
    QString fileName = QFileDialog::getOpenFileName(&window, 
        "Open Audio File", 
        QDir::homePath(),
        "Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a)");
    
    if (!fileName.isEmpty()) {
      player->setSource(QUrl::fromLocalFile(fileName));
      player->play();
    }
  });

  QObject::connect(playButton, &QPushButton::clicked, [player]() {
    player->play();
  });

  QObject::connect(pauseButton, &QPushButton::clicked, [player]() {
    player->pause();
  });

  QObject::connect(stopButton, &QPushButton::clicked, [player]() {
    player->stop();
  });

  QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);
  QObject::connect(fullscreenAction, &QAction::triggered, [&window]() {
    if (window.isFullScreen()) {
      window.showNormal();
    } else {
      window.showFullScreen();
    }
  });

  QObject::connect(minimizeAction, &QAction::triggered, [&window]() {
    window.showMinimized();
  });

window.showFullScreen();
  window.show();

  int result = app.exec();
  
  // Cleanup
  delete player;
  delete audioOutput;
  
  return result;
}
