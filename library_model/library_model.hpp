#ifndef LIBRARY
#define LIBRARY

#include "qobject.h"
#include "qtmetamacros.h"
#include <QObject>

class Library : public QObject{
    Q_OBJECT

    public:
    explicit Library(QObject* parent = nullptr);

    void addSongs();

};


#endif // !LIBRARY
