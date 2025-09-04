#ifndef QUEUE
#define QUEUE

#include "qaudiooutput.h"
#include "qmediaplayer.h"
#include "qobject.h"
#include "queue_model.hpp"
#include <QObject>
#include <memory>
#include <track/track.hpp>

class Queue : public QObject {
  Q_OBJECT

public:
  explicit Queue(QObject *parent = nullptr);

  // Get the model for the QListView
  QueueModel *getModel();
  std::shared_ptr<Track> getCurrentTrack();
  QMediaPlayer* getPlayer() const { return player; }
  
  int getCurrentIndex() const { return index; }
  void playAt(int trackIndex);

signals:

  void playbackStarted();
  void playbackPaused();
  void playbackStopped();
  void positionChanged();
  void currentIndexChanged(int newIndex);

public slots:
  void next();
  void previous();

  /// amount of milliseconds to seek. Negative is backwards
  void seek(int milliseconds);
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

private slots:
  void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
  /// Represents the currently selected/playing song in the queue. -1 means
  /// nothing selected.
  int index;
  QueueModel *queueModel;

  QMediaPlayer *player;
  QAudioOutput *audioOutput;
  
  // Flag to prevent auto-advance when manually changing tracks
  bool isManualAdvance = false;
};

#endif // !QUEUE
