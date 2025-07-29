#ifndef CONTROLLER
#define CONTROLLER

#include "library/library.hpp"
#include "qobject.h"
#include "qtmetamacros.h"
#include <QObject>

class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(Library *library, QObject *parent = nullptr);

signals:
  void scanLibraryUpdate(const std::string &message);

public slots:
  void scanLibrary();

private:
  Library *library;
};

#endif // CONTROLLER
