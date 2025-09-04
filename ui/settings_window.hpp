#ifndef SETTINGS_WINDOW
#define SETTINGS_WINDOW

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>

class SettingsWindow : public QDialog {
  Q_OBJECT

public:
  explicit SettingsWindow(QWidget *parent = nullptr);
  
  QStringList getLibraryPaths() const { return libraryPaths; }
  void setLibraryPaths(const QStringList &paths);

signals:
  void libraryPathsChanged(const QStringList &paths);

private slots:
  void addLibraryPath();
  void removeLibraryPath();
  void browseForPath();
  void saveSettings();
  void loadSettings();

private:
  void setupUI();
  void connectSignals();
  void updateListWidget();

  QVBoxLayout *mainLayout;
  QGroupBox *libraryGroup;
  QVBoxLayout *libraryLayout;
  QHBoxLayout *pathInputLayout;
  QLineEdit *pathInput;
  QPushButton *browseButton;
  QPushButton *addButton;
  QPushButton *removeButton;
  QListWidget *pathList;
  
  QHBoxLayout *buttonLayout;
  QPushButton *saveButton;
  QPushButton *cancelButton;
  
  QStringList libraryPaths;
};

#endif // !SETTINGS_WINDOW
