#include "controller/controller.hpp"
#include "library/library.hpp"
#include "library/library_model.hpp"
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
#include <QTreeView>
#include <QSplitter>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Library  *library = new Library();
  Controller * controller = new Controller(library);

  QMediaPlayer *player = new QMediaPlayer();
  QAudioOutput *audioOutput = new QAudioOutput();
  player->setAudioOutput(audioOutput);
  
  audioOutput->setVolume(0.5);

  QMainWindow window;
  window.setWindowTitle("ssmp");

  QWidget *centralWidget = new QWidget;
  window.setCentralWidget(centralWidget);

  QVBoxLayout *vbox = new QVBoxLayout;
  

  QSplitter *splitter = new QSplitter(Qt::Horizontal);
  
  QTreeView *libraryTreeView = new QTreeView();
  libraryTreeView->setModel(library->getModel());
  libraryTreeView->setHeaderHidden(true);
  libraryTreeView->setMinimumWidth(250);
  libraryTreeView->setMaximumWidth(600);
  
  // Main content area placeholder
  QLabel *mainContentLabel = new QLabel("Main window");
  mainContentLabel->setAlignment(Qt::AlignCenter);
  
  // Add widgets to splitter
  splitter->addWidget(libraryTreeView);
  splitter->addWidget(mainContentLabel);
  
  // Set splitter proportions (25% sidebar, 75% main)
  splitter->setSizes({250, 750});

  QFrame *mediacontrol_frame = new QFrame();

  QHBoxLayout *mediacontrols_hbox = new QHBoxLayout();
  mediacontrol_frame->setLayout(mediacontrols_hbox);

  mediacontrol_frame->setObjectName("mediacontrol");
  mediacontrol_frame->setStyleSheet(
      "#mediacontrol { border: 2px solid black; }");

  QPushButton *playButton = new QPushButton("Play");
  QPushButton *pauseButton = new QPushButton("Pause");
  QPushButton *stopButton = new QPushButton("Stop");
  
  // Add status label for library updates
  QLabel *statusLabel = new QLabel("Ready");
  statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
  
  mediacontrols_hbox->addWidget(playButton);
  mediacontrols_hbox->addWidget(pauseButton);
  mediacontrols_hbox->addWidget(stopButton);
  mediacontrols_hbox->addWidget(statusLabel);

  vbox->addWidget(splitter);
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
  QAction *fullScanAction = editMenu->addAction("&Full scan");
  editMenu->addSeparator();
  QAction *cutAction = editMenu->addAction("&Play song");
  
  // View menu
  QMenu *viewMenu = menuBar->addMenu("&View");
  QAction *fullscreenAction = viewMenu->addAction("&Fullscreen");
  QAction *minimizeAction = viewMenu->addAction("&Minimize");
  

 QObject::connect(cutAction, &QAction::triggered, [&window, player]() {
    QString musicFile = "/home/kayasem/Music/Library/song.mp3";
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

  QObject::connect(fullScanAction, &QAction::triggered, controller, &Controller::scanLibrary);

  QObject::connect(controller, &Controller::scanLibraryUpdate, 
                   [statusLabel](const std::string& message) {
                     statusLabel->setText(QString::fromStdString(message));
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

  // Connect tree view double-click to play track
  QObject::connect(libraryTreeView, &QTreeView::doubleClicked, 
                   [libraryTreeView, player](const QModelIndex& index) {
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(libraryTreeView->model());
    if (!model) return;
    
    QStandardItem* item = model->itemFromIndex(index);
    if (!item) return;
    
    // - No parent = Artist (top level)
    // - Parent has no parent = Album
    // - Parent's parent exists = Track
    QStandardItem* parent = item->parent();
    bool isTrack = parent && parent->parent();
    
    if (isTrack) {
      TrackItem* trackItem = static_cast<TrackItem*>(item);
      QString filePath = trackItem->getFilePath();
      if (!filePath.isEmpty()) {
        player->setSource(QUrl::fromLocalFile(filePath));
        player->play();
      }
    }
  });

window.showFullScreen();
  window.show();

  int result = app.exec();
  
  delete player;
  delete audioOutput;
  
  return result;
}
