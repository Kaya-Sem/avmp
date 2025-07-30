#include "queue.hpp"
#include "qaudiooutput.h"
#include "qlist.h"
#include "qmediaplayer.h"
#include "qobject.h"
#include <QDebug>
#include <memory>

Queue::Queue(QObject *_) {
  this->index = -1;
  this->trackQueue_list = new QList<std::shared_ptr<Track>>;

  this->player = new QMediaPlayer();
  this->audioOutput = new QAudioOutput();

  player->setAudioOutput(audioOutput);

  audioOutput->setVolume(0.5);
};

void Queue::next() {
  if (index < trackQueue_list->length()) {
    index += 1;

    std::string path = trackQueue_list->at(index)->fullPath;
    player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

    player->play();
  }
}

void Queue::appendTrack(std::shared_ptr<Track> track) {
  trackQueue_list->append(track);
}

void Queue::playNext(std::shared_ptr<Track> track) {
  trackQueue_list->insert(index + 1, track);
}

void Queue::pause() {
  player->pause();
  emit Queue::playbackPaused();
}

void Queue::play() {
  player->play();
  emit Queue::playbackStarted();
}

void Queue::previous() {
  if (index > 0) {
    index -= 1;

    std::string path = trackQueue_list->at(index)->fullPath;
    player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

    player->play();
  }
}

void Queue::playNow(std::shared_ptr<Track> track) {
  trackQueue_list->insert(index + 1, track);
  index += 1;

  std::string path = trackQueue_list->at(index)->fullPath;
  player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

  player->play();
}

void Queue::seek(int value) {
  qint64 currentPos = player->position();
  qint64 duration = player->duration();
  qint64 newPos = currentPos + value;

  if (value < 0) {
    // Clamp to minimum 0
    newPos = qMax(qint64(0), newPos);
  } else {
    // Clamp to maximum duration
    newPos = qMin(duration, newPos);
  }

  player->setPosition(newPos);
}

void Queue::stop() { player->stop(); }

void Queue::clear() {
  player->stop();

  trackQueue_list->clear();
}
