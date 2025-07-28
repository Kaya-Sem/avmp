#include "controller.hpp"
#include "qdebug.h"
#include "qobject.h"

Controller::Controller(Library *library, QObject *parent) : QObject(parent) {
    // Store reference to library if needed later
    // this->library = library;
}

void Controller::scanLibrary() { 
    qDebug() << "Scanning library..."; 
}
