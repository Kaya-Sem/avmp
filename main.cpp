#include "context/player_context.hpp"
#include "controller/controller.hpp"
#include "library/library.hpp"
#include "qlistview.h"
#include "queue/queue.hpp"
#include "qwidget.h"
#include "ui/mediacontrol/mediacontrol.hpp"
#include "ui/queue_listview.hpp"
#include "ui/settings_window.hpp"
#include "ui/tabs/collection_treeview_tab.hpp"
#include "ui/tabs/track_context_tab.hpp"
#include <QApplication>
#include <QAudioOutput>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

// Global terminal widget for logging
QPlainTextEdit *g_terminalWidget = nullptr;

void logToTerminal(const QString &message) {
  if (g_terminalWidget) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logMessage = QString("%1 %2").arg(timestamp, message);
    g_terminalWidget->appendPlainText(logMessage);

    // Auto-scroll to bottom
    QTextCursor cursor = g_terminalWidget->textCursor();
    cursor.movePosition(QTextCursor::End);
    g_terminalWidget->setTextCursor(cursor);
  }
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Library *library = new Library();
  Controller *controller = new Controller(library);

  Queue *queue = new Queue();
  auto ctx = PlayerContext::instance();
  ctx->initialize(controller, library, queue);

  QMainWindow window;
  window.setWindowTitle("avmp");

  QWidget *centralWidget = new QWidget;
  window.setCentralWidget(centralWidget);

  QVBoxLayout *vbox = new QVBoxLayout;

  QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);

  // Left tabbed pane
  QTabWidget *leftTabWidget = new QTabWidget();
  leftTabWidget->setMinimumWidth(250);
  leftTabWidget->setMaximumWidth(700);
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");

  QListView * playlistList = new QListView();
  playlistList->setFrameStyle(QFrame::NoFrame);
  leftTabWidget->addTab(playlistList, "Playlists");


  QTabWidget * centerPanel = new QTabWidget();

  // Center content area - Queue list
  QueueListView *queueListView = new QueueListView();
  centerPanel->addTab(queueListView, "Queue");
  QWidget * rockplaylist = new QWidget();
  centerPanel->addTab(rockplaylist, "Lovely Rock");

  // Right tabbed pane
  QTabWidget *rightTabWidget = new QTabWidget();
  rightTabWidget->setMinimumWidth(300);
  rightTabWidget->setMaximumWidth(700);

  // Terminal tab
  QWidget *terminalTabWidget = new QWidget();
  QVBoxLayout *terminalLayout = new QVBoxLayout();

  QPlainTextEdit *terminalWidget = new QPlainTextEdit();

  // Terminal controls
  QHBoxLayout *terminalControlsLayout = new QHBoxLayout();
  QPushButton *clearTerminalButton = new QPushButton("Clear Terminal");
  QPushButton *saveLogButton = new QPushButton("Save Log");

  terminalControlsLayout->addWidget(clearTerminalButton);
  terminalControlsLayout->addWidget(saveLogButton);
  terminalControlsLayout->addStretch();

  terminalLayout->addWidget(terminalWidget);
  terminalLayout->addLayout(terminalControlsLayout);
  terminalTabWidget->setLayout(terminalLayout);

  g_terminalWidget = terminalWidget;

  logToTerminal("Terminal initialized");

  rightTabWidget->addTab(new TrackContextTab(), "Track");
  rightTabWidget->addTab(terminalTabWidget, "Terminal");

  mainSplitter->addWidget(leftTabWidget);
  mainSplitter->addWidget(centerPanel);
  mainSplitter->addWidget(rightTabWidget);

  // proportions (20% left, 50% center, 30% right)
  mainSplitter->setSizes({250, 500, 300});

  vbox->addWidget(mainSplitter);
  vbox->addWidget(new MediaControls());

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

  // Tools menu
  QMenu *toolsMenu = menuBar->addMenu("&Tools");
  QAction *settingsAction = toolsMenu->addAction("&Settings");

  QObject::connect(cutAction, &QAction::triggered, [&queue]() {
    QString musicFile = "/home/kayasem/Music/Library/song.mp3";

    std::shared_ptr<Track> track = std::make_shared<Track>(musicFile);
    queue->playNow(track);
    logToTerminal("Playing test song: " + musicFile);
  });

  QObject::connect(openAction, &QAction::triggered, [&window, queue]() {
    QString fileName = QFileDialog::getOpenFileName(
        &window, "Open Audio File", QDir::homePath(),
        "Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a)");

    if (!fileName.isEmpty()) {
      std::shared_ptr<Track> track = std::make_shared<Track>(fileName);
      queue->playNow(track);
      logToTerminal("Opened and queued audio file: " + fileName);
    }
  });

  QObject::connect(fullScanAction, &QAction::triggered, [controller]() {
    logToTerminal("Starting library scan...");
    controller->scanLibrary();
  });

  QObject::connect(settingsAction, &QAction::triggered, [&window, controller]() {
    SettingsWindow settingsWindow(&window);
    settingsWindow.setLibraryPaths(controller->getLibraryPaths());
    
    if (settingsWindow.exec() == QDialog::Accepted) {
      QStringList libraryPaths = settingsWindow.getLibraryPaths();
      controller->setLibraryPaths(libraryPaths);
      logToTerminal("Settings saved. Library paths: " + libraryPaths.join(", "));
    }
  });

  // Terminal button connections
  QObject::connect(clearTerminalButton, &QPushButton::clicked,
                   [terminalWidget]() {
                     terminalWidget->clear();
                     logToTerminal("Terminal cleared");
                   });

  QObject::connect(
      saveLogButton, &QPushButton::clicked, [&window, terminalWidget]() {
        QString fileName = QFileDialog::getSaveFileName(
            &window, "Save Log File", QDir::homePath() + "/ssmp_log.txt",
            "Text Files (*.txt);;All Files (*)");

        if (!fileName.isEmpty()) {
          QFile file(fileName);
          if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << terminalWidget->toPlainText();
            file.close();
            logToTerminal("Log saved to: " + fileName);
          } else {
            logToTerminal("Failed to save log file");
          }
        }
      });

  QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);
  QObject::connect(fullscreenAction, &QAction::triggered, [&window]() {
    if (window.isFullScreen()) {
      window.showNormal();
    } else {
      window.showFullScreen();
    }
  });

  QObject::connect(minimizeAction, &QAction::triggered,
                   [&window]() { window.showMinimized(); });

  window.showFullScreen();
  window.show();

  int result = app.exec();

  delete queue;

  return result;
}
