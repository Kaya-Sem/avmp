#include "context/player_context.hpp"
#include "controller/controller.hpp"
#include "library/library.hpp"
#include "library/playlist_manager.hpp"
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
#include <QInputDialog>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
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

  // Playlists tab
  QWidget *playlistTab = new QWidget();
  QVBoxLayout *playlistLayout = new QVBoxLayout();

  QTreeView *playlistTree = new QTreeView();
  playlistTree->setFrameStyle(QFrame::NoFrame);
  playlistTree->setHeaderHidden(true);
  PlaylistManager *pm = ctx->playlistManager();
  playlistTree->setModel(pm->getModel());
  playlistTree->setContextMenuPolicy(Qt::CustomContextMenu);

  QPushButton *newPlaylistBtn = new QPushButton("New Playlist");
  playlistLayout->addWidget(playlistTree);
  playlistLayout->addWidget(newPlaylistBtn);
  playlistTab->setLayout(playlistLayout);
  leftTabWidget->addTab(playlistTab, "Playlists");

  QTabWidget * centerPanel = new QTabWidget();

  // Center content area - Queue list
  QueueListView *queueListView = new QueueListView();
  centerPanel->addTab(queueListView, "Queue");

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
  QAction *openAction = fileMenu->addAction("&Open");
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

  // Playlist UI connections
  QObject::connect(newPlaylistBtn, &QPushButton::clicked, [&window, pm]() {
    bool ok;
    QString name = QInputDialog::getText(&window, "New Playlist",
                                         "Playlist name:", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty()) {
      pm->createPlaylist(name);
      logToTerminal("Created playlist: " + name);
    }
  });

  QObject::connect(playlistTree, &QTreeView::doubleClicked,
                   [pm, queue, centerPanel](const QModelIndex &index) {
    // If it's a top-level item (playlist), load all tracks into queue
    if (!index.parent().isValid()) {
      QString name = index.data().toString();
      auto playlist = pm->getPlaylist(name);
      if (!playlist)
        return;

      queue->clear();
      for (const auto &track : playlist->getTracks()) {
        queue->appendTrack(track);
      }
      centerPanel->setCurrentIndex(0);
      logToTerminal("Loaded playlist: " + name);
    } else {
      // It's a track item — play just that track
      QString filePath = index.data(Qt::UserRole + 1).toString();
      if (!filePath.isEmpty()) {
        auto track = std::make_shared<Track>(filePath);
        queue->playNow(track);
      }
    }
  });

  QObject::connect(playlistTree, &QTreeView::customContextMenuRequested,
                   [&window, playlistTree, pm](const QPoint &pos) {
    QModelIndex index = playlistTree->indexAt(pos);
    if (!index.isValid())
      return;

    // Only show context menu on top-level playlist items
    if (index.parent().isValid())
      return;

    QString name = index.data().toString();
    QMenu menu(playlistTree);

    menu.addAction("Delete Playlist", [&window, pm, name]() {
      auto result = QMessageBox::question(&window, "Delete Playlist",
          "Delete playlist \"" + name + "\"? This will remove all hardlinked files.",
          QMessageBox::Yes | QMessageBox::No);
      if (result == QMessageBox::Yes) {
        pm->deletePlaylist(name);
        logToTerminal("Deleted playlist: " + name);
      }
    });

    menu.addAction("Rename Playlist", [&window, pm, name]() {
      bool ok;
      QString newName = QInputDialog::getText(&window, "Rename Playlist",
                                              "New name:", QLineEdit::Normal,
                                              name, &ok);
      if (ok && !newName.isEmpty() && newName != name) {
        auto playlist = pm->getPlaylist(name);
        if (playlist) {
          pm->createPlaylist(newName);
          auto newPlaylist = pm->getPlaylist(newName);
          if (newPlaylist) {
            for (const auto &track : playlist->getTracks()) {
              newPlaylist->addTrack(*track);
            }
          }
          pm->deletePlaylist(name);
          logToTerminal("Renamed playlist: " + name + " -> " + newName);
        }
      }
    });

    menu.exec(playlistTree->viewport()->mapToGlobal(pos));
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
