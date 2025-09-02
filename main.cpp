#include "controller/controller.hpp"
#include "library/library.hpp"
#include "context/player_context.hpp"
#include "ui/tabs/collection_treeview_tab.hpp"
#include "qframe.h"
#include "qlabel.h"
#include "queue/queue.hpp"
#include <QApplication>
#include <QAudioOutput>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <memory>

// Global terminal widget for logging
QPlainTextEdit* g_terminalWidget = nullptr;

void logToTerminal(const QString& message) {
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
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");
  leftTabWidget->addTab(new CollectionTreeviewTab, "Collection");

  // Center content area - Queue list
  QListView *queueListView = new QListView();
  queueListView->setModel(queue->getModel());

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
  
  rightTabWidget->addTab(terminalTabWidget, "Terminal");
  
  // Tab 2 - Queue Information
  QWidget *tab2Widget = new QWidget();
  QVBoxLayout *tab2Layout = new QVBoxLayout();
  QLabel *tab2Label = new QLabel("Queue Information");
  tab2Label->setStyleSheet("QLabel { font-weight: bold; font-size: 14pt; }");
  tab2Label->setAlignment(Qt::AlignCenter);
  
  QLabel *queueStatusLabel = new QLabel("Queue Status: Empty");
  QLabel *currentTrackLabel = new QLabel("Current Track: None");
  QLabel *queueLengthLabel = new QLabel("Queue Length: 0");
  
  tab2Layout->addWidget(tab2Label);
  tab2Layout->addWidget(queueStatusLabel);
  tab2Layout->addWidget(currentTrackLabel);
  tab2Layout->addWidget(queueLengthLabel);
  tab2Layout->addStretch();
  tab2Widget->setLayout(tab2Layout);
  rightTabWidget->addTab(tab2Widget, "Queue Info");


  mainSplitter->addWidget(leftTabWidget);
  mainSplitter->addWidget(queueListView);
  mainSplitter->addWidget(rightTabWidget);

  // proportions (20% left, 50% center, 30% right)
  mainSplitter->setSizes({250, 500, 300});

  QFrame *mediacontrol_frame = new QFrame();

  QHBoxLayout *mediacontrols_hbox = new QHBoxLayout();
  mediacontrol_frame->setLayout(mediacontrols_hbox);

  mediacontrol_frame->setObjectName("mediacontrol");

  QPushButton *playButton = new QPushButton("Play");
  QPushButton *pauseButton = new QPushButton("Pause");
  QPushButton *stopButton = new QPushButton("Stop");

  QLabel *statusLabel = new QLabel("Ready");
  statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");

  mediacontrols_hbox->addWidget(playButton);
  mediacontrols_hbox->addWidget(pauseButton);
  mediacontrols_hbox->addWidget(stopButton);
  mediacontrols_hbox->addWidget(statusLabel);

  vbox->addWidget(mainSplitter);
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

  QObject::connect(playButton, &QPushButton::clicked,
                   [&queue]() { 
                     queue->play(); 
                     logToTerminal("Play button clicked");
                   });

  QObject::connect(fullScanAction, &QAction::triggered, [controller]() {
    logToTerminal("Starting library scan...");
    controller->scanLibrary();
  });

  QObject::connect(controller, &Controller::scanLibraryUpdate,
                   [statusLabel](const std::string &message) {
                     QString qMessage = QString::fromStdString(message);
                     statusLabel->setText(qMessage);
                     logToTerminal(qMessage);
                   });

  QObject::connect(pauseButton, &QPushButton::clicked,
                   [&queue]() { 
                     queue->pause(); 
                     logToTerminal("Pause button clicked");
                   });

  QObject::connect(stopButton, &QPushButton::clicked,
                   [&queue]() { 
                     queue->stop(); 
                     logToTerminal("Stop button clicked");
                   });

  // Terminal button connections
  QObject::connect(clearTerminalButton, &QPushButton::clicked, [terminalWidget]() {
    terminalWidget->clear();
    logToTerminal("Terminal cleared");
  });
  
  QObject::connect(saveLogButton, &QPushButton::clicked, [&window, terminalWidget]() {
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
