#ifndef CONTROLLER
#define CONTROLLER

#include "library/library.hpp"
#include "qobject.h"
#include <QObject>
#include <QStringList>

class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(Library *library, QObject *parent = nullptr);
  
  void setLibraryPaths(const QStringList &paths);
  QStringList getLibraryPaths() const { return libraryPaths; }

signals:
  void scanLibraryUpdate(const std::string &message);

public slots:
  void scanLibrary();

private:
  Library *library;
  QStringList libraryPaths;
};

#endif // CONTROLLER
