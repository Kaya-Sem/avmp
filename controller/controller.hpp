#ifndef CONTROLLER
#define CONTROLLER

#include "library/library.hpp"
#include "qobject.h"
#include <QObject>
#include <QStringList>
#include <memory>
#include <vector>
#include "track/track.hpp"

class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(Library *library, QObject *parent = nullptr);

  void setLibraryPaths(const QStringList &paths);
  QStringList getLibraryPaths() const { return libraryPaths; }

signals:
  void scanLibraryUpdate(const std::string &message);
  void scanLibraryFinished(const std::vector<std::shared_ptr<Track>> &tracks);

public slots:
  void scanLibrary();
  void scanLibraryAsync();

private:
  Library *library;
  QStringList libraryPaths;
};

#endif // CONTROLLER
