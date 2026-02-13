#include "track_context_tab.hpp"
#include "library/library_model.hpp"
#include <QDateTime>
#include <QFileInfo>
#include <QFont>
#include <QPixmap>

TrackContextTab::TrackContextTab(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // Album art
  albumArtLabel = new QLabel();
  albumArtLabel->setFixedSize(250, 250);
  albumArtLabel->setAlignment(Qt::AlignCenter);
  albumArtLabel->setStyleSheet(
      "background-color: #2a2a2a; border: 1px solid #3a3a3a;");
  mainLayout->addWidget(albumArtLabel, 0, Qt::AlignHCenter);

  // Scroll area for metadata
  QScrollArea *scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);

  QWidget *scrollContent = new QWidget();
  QFormLayout *form = new QFormLayout(scrollContent);
  form->setLabelAlignment(Qt::AlignRight);
  form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

  // Helper to create a row
  auto addRow = [&](const QString &label, QLabel *&rowLabel,
                    QLabel *&valueLabel) {
    rowLabel = new QLabel(label);
    rowLabel->setStyleSheet("font-weight: bold; color: #aaa;");
    valueLabel = new QLabel();
    valueLabel->setWordWrap(true);
    valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    form->addRow(rowLabel, valueLabel);
  };

  // Track Info section
  addSectionHeader(form, "Track Info");
  addRow("Title", titleRowLabel, titleLabel);
  addRow("Artist", artistRowLabel, artistLabel);
  addRow("Album Artist", albumArtistRowLabel, albumArtistLabel);
  addRow("Album", albumRowLabel, albumLabel);
  addRow("Year", yearRowLabel, yearLabel);
  addRow("Track #", trackNumRowLabel, trackNumLabel);
  addRow("Disc #", discNumRowLabel, discNumLabel);
  addRow("Genre", genreRowLabel, genreLabel);
  addRow("Composer", composerRowLabel, composerLabel);
  addRow("BPM", bpmRowLabel, bpmLabel);
  addRow("Compilation", compilationRowLabel, compilationLabel);
  addRow("Comment", commentRowLabel, commentLabel);
  addRow("Lyrics", lyricsRowLabel, lyricsLabel);

  // Audio section
  addSectionHeader(form, "Audio");
  addRow("Duration", durationRowLabel, durationLabel);
  addRow("Bitrate", bitrateRowLabel, bitrateLabel);
  addRow("Sample Rate", sampleRateRowLabel, sampleRateLabel);
  addRow("Channels", channelsRowLabel, channelsLabel);
  addRow("Codec", codecRowLabel, codecLabel);

  // File section
  addSectionHeader(form, "File");
  addRow("Path", pathRowLabel, pathLabel);
  addRow("Size", sizeRowLabel, sizeLabel);
  addRow("Modified", modifiedRowLabel, modifiedLabel);

  scrollArea->setWidget(scrollContent);
  mainLayout->addWidget(scrollArea);

  clearAll();
}

void TrackContextTab::addSectionHeader(QFormLayout *form,
                                       const QString &title) {
  QLabel *header = new QLabel(title);
  header->setStyleSheet(
      "font-weight: bold; font-size: 13px; color: #ddd; "
      "padding-top: 8px; padding-bottom: 2px; "
      "border-bottom: 1px solid #444;");
  form->addRow(header);
}

void TrackContextTab::setRowVisible(QLabel *label, QLabel *value,
                                    bool visible) {
  label->setVisible(visible);
  value->setVisible(visible);
}

void TrackContextTab::clearAll() {
  albumArtLabel->clear();
  albumArtLabel->setText("No Track");
  albumArtLabel->setStyleSheet(
      "background-color: #2a2a2a; border: 1px solid #3a3a3a; color: #666;");

  auto clearLabel = [](QLabel *l) { l->clear(); };

  clearLabel(titleLabel);
  clearLabel(artistLabel);
  clearLabel(albumArtistLabel);
  clearLabel(albumLabel);
  clearLabel(yearLabel);
  clearLabel(trackNumLabel);
  clearLabel(discNumLabel);
  clearLabel(genreLabel);
  clearLabel(composerLabel);
  clearLabel(bpmLabel);
  clearLabel(commentLabel);
  clearLabel(compilationLabel);
  clearLabel(lyricsLabel);
  clearLabel(durationLabel);
  clearLabel(bitrateLabel);
  clearLabel(sampleRateLabel);
  clearLabel(channelsLabel);
  clearLabel(codecLabel);
  clearLabel(pathLabel);
  clearLabel(sizeLabel);
  clearLabel(modifiedLabel);
}

void TrackContextTab::updateTrack(std::shared_ptr<Track> track) {
  if (!track) {
    clearAll();
    return;
  }

  // Album artwork
  QPixmap artwork = extractAlbumArtwork(track->fullPath);
  if (!artwork.isNull()) {
    albumArtLabel->setPixmap(
        artwork.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    albumArtLabel->setStyleSheet("border: 1px solid #3a3a3a;");
  } else {
    albumArtLabel->clear();
    albumArtLabel->setText("No Artwork");
    albumArtLabel->setStyleSheet(
        "background-color: #2a2a2a; border: 1px solid #3a3a3a; color: #666; "
        "font-size: 14px;");
  }

  // Helper to set a string field and show/hide
  auto setField = [&](QLabel *rowLabel, QLabel *valueLabel,
                      const std::string &val) {
    bool hasValue = !val.empty();
    setRowVisible(rowLabel, valueLabel, hasValue);
    if (hasValue)
      valueLabel->setText(QString::fromStdString(val));
  };

  // Helper for int fields (hide if 0)
  auto setIntField = [&](QLabel *rowLabel, QLabel *valueLabel, int val,
                         const QString &suffix = "") {
    bool hasValue = val > 0;
    setRowVisible(rowLabel, valueLabel, hasValue);
    if (hasValue)
      valueLabel->setText(QString::number(val) + suffix);
  };

  // Track Info
  setField(titleRowLabel, titleLabel, track->title);
  setField(artistRowLabel, artistLabel, track->artist);
  setField(albumArtistRowLabel, albumArtistLabel, track->albumArtist);
  setField(albumRowLabel, albumLabel, track->album);
  setIntField(yearRowLabel, yearLabel, track->year);
  setIntField(trackNumRowLabel, trackNumLabel, track->track);
  setIntField(discNumRowLabel, discNumLabel, track->discNumber);
  setField(genreRowLabel, genreLabel, track->genre);
  setField(composerRowLabel, composerLabel, track->composer);
  setIntField(bpmRowLabel, bpmLabel, track->bpm);

  // Compilation
  if (track->compilation) {
    setRowVisible(compilationRowLabel, compilationLabel, true);
    compilationLabel->setText("Yes");
  } else {
    setRowVisible(compilationRowLabel, compilationLabel, false);
  }

  setField(commentRowLabel, commentLabel, track->comment);
  setField(lyricsRowLabel, lyricsLabel, track->lyrics);

  // Audio
  if (track->duration > 0) {
    int mins = track->duration / 60;
    int secs = track->duration % 60;
    durationLabel->setText(
        QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0')));
    setRowVisible(durationRowLabel, durationLabel, true);
  } else {
    setRowVisible(durationRowLabel, durationLabel, false);
  }

  setIntField(bitrateRowLabel, bitrateLabel, track->bitrate, " kbps");
  setIntField(sampleRateRowLabel, sampleRateLabel, track->sampleRate, " Hz");

  if (track->channels > 0) {
    QString chStr = track->channels == 1   ? "Mono"
                    : track->channels == 2 ? "Stereo"
                                           : QString::number(track->channels) + " ch";
    channelsLabel->setText(chStr);
    setRowVisible(channelsRowLabel, channelsLabel, true);
  } else {
    setRowVisible(channelsRowLabel, channelsLabel, false);
  }

  setField(codecRowLabel, codecLabel, track->codec);

  // File info
  QFileInfo fileInfo(QString::fromStdString(track->fullPath));
  pathLabel->setText(QString::fromStdString(track->fullPath));
  setRowVisible(pathRowLabel, pathLabel, true);

  if (fileInfo.exists()) {
    // File size (human-readable)
    qint64 bytes = fileInfo.size();
    QString sizeStr;
    if (bytes >= 1073741824)
      sizeStr = QString::number(bytes / 1073741824.0, 'f', 1) + " GB";
    else if (bytes >= 1048576)
      sizeStr = QString::number(bytes / 1048576.0, 'f', 1) + " MB";
    else if (bytes >= 1024)
      sizeStr = QString::number(bytes / 1024.0, 'f', 1) + " KB";
    else
      sizeStr = QString::number(bytes) + " B";
    sizeLabel->setText(sizeStr);
    setRowVisible(sizeRowLabel, sizeLabel, true);

    modifiedLabel->setText(
        fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
    setRowVisible(modifiedRowLabel, modifiedLabel, true);
  } else {
    setRowVisible(sizeRowLabel, sizeLabel, false);
    setRowVisible(modifiedRowLabel, modifiedLabel, false);
  }
}
