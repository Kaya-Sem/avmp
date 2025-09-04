#include "queue.hpp"
#include "qaudiooutput.h"
#include "qlist.h"
#include "qmediaplayer.h"
#include "qobject.h"
#include <QDebug>
#include <memory>

Queue::Queue(QObject *parent) : QObject(parent) {
  this->index = -1;
  this->queueModel = new QueueModel(this);

  this->player = new QMediaPlayer();
  this->audioOutput = new QAudioOutput();

  player->setAudioOutput(audioOutput);

  audioOutput->setVolume(0.5);
  
  // Connect currentIndexChanged signal to update the model
  connect(this, &Queue::currentIndexChanged, queueModel, &QueueModel::setCurrentIndex);
  
  // Connect player signals for auto-advance
  connect(player, &QMediaPlayer::playbackStateChanged, this, &Queue::onPlaybackStateChanged);
};

QueueModel* Queue::getModel() {
  return queueModel;
}

void Queue::next() {
  const auto& trackList = queueModel->getTrackList();
  if (index < trackList.length() - 1) {
    isManualAdvance = true;  // Set flag to prevent auto-advance
    index += 1;

    std::string path = trackList.at(index)->fullPath;
    player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

    player->play();
    emit currentIndexChanged(index);
  }
}

void Queue::appendTrack(std::shared_ptr<Track> track) {
  queueModel->appendTrack(track);
}

void Queue::playNext(std::shared_ptr<Track> track) {
  queueModel->insertTrack(track, index + 1);
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
    isManualAdvance = true;  // Set flag to prevent auto-advance
    index -= 1;

    const auto& trackList = queueModel->getTrackList();
    std::string path = trackList.at(index)->fullPath;
    player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

    player->play();
    emit currentIndexChanged(index);
  }
}

void Queue::playNow(std::shared_ptr<Track> track) {
  isManualAdvance = true;  // Set flag to prevent auto-advance
  queueModel->insertTrack(track, index + 1);
  index += 1;

  const auto& trackList = queueModel->getTrackList();
  std::string path = trackList.at(index)->fullPath;
  player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

  player->play();
  emit currentIndexChanged(index);
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

void Queue::stop() { 
  player->stop(); 
}

std::shared_ptr<Track> Queue::getCurrentTrack() {
  return queueModel->getTrack(index);

}

void Queue::clear() {
  player->stop();
  queueModel->clear();
  index = -1;
  emit currentIndexChanged(index);
}

void Queue::playAt(int trackIndex) {
  const auto& trackList = queueModel->getTrackList();
  if (trackIndex >= 0 && trackIndex < trackList.length()) {
    isManualAdvance = true;  // Set flag to prevent auto-advance
    index = trackIndex;
    
    std::string path = trackList.at(index)->fullPath;
    player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));
    
    player->play();
    emit currentIndexChanged(index);
  }
}

void Queue::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
  // When playback stops (track finished), automatically go to next track
  if (state == QMediaPlayer::StoppedState) {
    // Only auto-advance if this wasn't a manual track change
    if (!isManualAdvance) {
      const auto& trackList = queueModel->getTrackList();
      // Only auto-advance if we're not at the end of the queue
      if (index >= 0 && index < trackList.length() - 1) {
        index += 1;

        std::string path = trackList.at(index)->fullPath;
        player->setSource(QUrl::fromLocalFile(QString::fromStdString(path)));

        player->play();
        emit currentIndexChanged(index);
      }
    } else {
      // Reset the flag after a manual advance
      isManualAdvance = false;
    }
  }
}
