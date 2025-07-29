#include "queue.hpp"
#include <QDebug>
#include "qlogging.h"

Queue::Queue() {};

void Queue::next() {
  qDebug() << "next called!";
}
