#include "library.hpp"
#include <QDebug>

Library::Library(QObject *parent) : QObject(parent) {
    // Initialize the library
}

void Library::addSongs() {
    qDebug() << "Adding songs to library...";
    // Implementation for adding songs will go here
}
