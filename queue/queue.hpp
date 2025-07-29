#ifndef QUEUE
#define QUEUE

#include "qlist.h"
#include <QObject>
#include <track/track.hpp>

class Queue : public QObject {
  Q_OBJECT

public:
  explicit Queue();

signals:

  void playbackStarted();
  void playbackPaused();
  void playbackStopped();
  void positionChanged();

public slots:
  void next();
  void previous();
  void clear();
  void seek();
  void play();
  void pause();

  void addTrack();
  void stop();

private:
  const QList<Track> *trackQueue;
};

#endif // !QUEUE
