#ifndef CONTROLLER
#define CONTROLLER

#include "library_model/library_model.hpp"
#include "qobject.h"
#include "qtmetamacros.h"
#include <QObject>

class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(Library *library, QObject *parent = nullptr);

public slots:
  void scanLibrary();
};

#endif // CONTROLLER
