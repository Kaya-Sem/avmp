#include "settings_window.hpp"
#include <QApplication>
#include <QSettings>
#include <QDir>

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Settings");
  setModal(true);
  setMinimumSize(500, 400);
  
  setupUI();
  connectSignals();
  loadSettings();
}

void SettingsWindow::setupUI() {
  mainLayout = new QVBoxLayout(this);
  
  // Library paths group
  libraryGroup = new QGroupBox("Library Paths", this);
  libraryLayout = new QVBoxLayout(libraryGroup);
  
  // Path input section
  pathInputLayout = new QHBoxLayout();
  pathInput = new QLineEdit(this);
  pathInput->setPlaceholderText("Enter library path or click Browse...");
  browseButton = new QPushButton("Browse...", this);
  addButton = new QPushButton("Add", this);
  
  pathInputLayout->addWidget(pathInput);
  pathInputLayout->addWidget(browseButton);
  pathInputLayout->addWidget(addButton);
  
  // Path list section
  pathList = new QListWidget(this);
  pathList->setSelectionMode(QAbstractItemView::SingleSelection);
  pathList->setFrameStyle(QFrame::NoFrame);
  
  removeButton = new QPushButton("Remove Selected", this);
  removeButton->setEnabled(false);
  
  // Button layout
  buttonLayout = new QHBoxLayout();
  saveButton = new QPushButton("Save", this);
  cancelButton = new QPushButton("Cancel", this);
  
  buttonLayout->addStretch();
  buttonLayout->addWidget(saveButton);
  buttonLayout->addWidget(cancelButton);
  
  // Assemble layout
  libraryLayout->addLayout(pathInputLayout);
  libraryLayout->addWidget(new QLabel("Library Paths:", this));
  libraryLayout->addWidget(pathList);
  libraryLayout->addWidget(removeButton);
  
  mainLayout->addWidget(libraryGroup);
  mainLayout->addLayout(buttonLayout);
}

void SettingsWindow::connectSignals() {
  connect(addButton, &QPushButton::clicked, this, &SettingsWindow::addLibraryPath);
  connect(browseButton, &QPushButton::clicked, this, &SettingsWindow::browseForPath);
  connect(removeButton, &QPushButton::clicked, this, &SettingsWindow::removeLibraryPath);
  connect(saveButton, &QPushButton::clicked, this, &SettingsWindow::saveSettings);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  
  connect(pathList, &QListWidget::itemSelectionChanged, [this]() {
    removeButton->setEnabled(pathList->currentItem() != nullptr);
  });
  
  connect(pathInput, &QLineEdit::returnPressed, this, &SettingsWindow::addLibraryPath);
}

void SettingsWindow::addLibraryPath() {
  QString path = pathInput->text().trimmed();
  if (path.isEmpty()) {
    QMessageBox::warning(this, "Invalid Path", "Please enter a valid path.");
    return;
  }
  
  QDir dir(path);
  if (!dir.exists()) {
    QMessageBox::warning(this, "Path Not Found", "The specified path does not exist.");
    return;
  }
  
  if (!libraryPaths.contains(path)) {
    libraryPaths.append(path);
    updateListWidget();
    pathInput->clear();
  } else {
    QMessageBox::information(this, "Duplicate Path", "This path is already in the list.");
  }
}

void SettingsWindow::removeLibraryPath() {
  QListWidgetItem *item = pathList->currentItem();
  if (item) {
    QString path = item->text();
    libraryPaths.removeAll(path);
    updateListWidget();
  }
}

void SettingsWindow::browseForPath() {
  QString path = QFileDialog::getExistingDirectory(this, "Select Library Directory");
  if (!path.isEmpty()) {
    pathInput->setText(path);
  }
}

void SettingsWindow::updateListWidget() {
  pathList->clear();
  for (const QString &path : libraryPaths) {
    pathList->addItem(path);
  }
}

void SettingsWindow::setLibraryPaths(const QStringList &paths) {
  libraryPaths = paths;
  updateListWidget();
}

void SettingsWindow::saveSettings() {
  QSettings settings;
  settings.setValue("libraryPaths", libraryPaths);
  emit libraryPathsChanged(libraryPaths);
  accept();
}

void SettingsWindow::loadSettings() {
  QSettings settings;
  libraryPaths = settings.value("libraryPaths", QStringList()).toStringList();
  
  // Add default path if no paths are configured
  if (libraryPaths.isEmpty()) {
    libraryPaths.append("/home/kayasem/Music");
  }
  
  updateListWidget();
}
