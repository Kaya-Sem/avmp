#ifndef QUEUE
#define QUEUE

#include "qaudiooutput.h"
#include "qlist.h"
#include "qmediaplayer.h"
#include "qobject.h"
#include <QObject>
#include <memory>
#include <track/track.hpp>

class Queue : public QObject {
  Q_OBJECT

public:
  explicit Queue(QObject *parent = nullptr);

signals:

  void playbackStarted();
  void playbackPaused();
  void playbackStopped();
  void positionChanged();

public slots:
  void next();
  void previous();

  /// amount of milliseconds to seek. Negative is backwards
  void seek(int value);
  void play();
  void pause();

  /// Adds track to the end of the queue
  void appendTrack(std::shared_ptr<Track> track);

  /// Adds track after current index in queue
  void playNext(std::shared_ptr<Track> track);
  void playNow(std::shared_ptr<Track> track);

  /// Stops playback and resets to beginning of track
  void stop();

  /// Clears the entire queue, which means emptying the vector
  void clear();

private:
  /// Represents the currently selected/playing song in the queue. -1 means
  /// nothing selected.
  int index;
  QList<std::shared_ptr<Track>> *trackQueue_list;

  QMediaPlayer *player;
  QAudioOutput *audioOutput;
};

#endif // !QUEUE
