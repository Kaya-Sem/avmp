#ifndef TRACK_CONTEXT_TAB
#define TRACK_CONTEXT_TAB

#include "track/track.hpp"
#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

class TrackContextTab : public QWidget {
  Q_OBJECT

public:
  explicit TrackContextTab(QWidget *parent = nullptr);

public slots:
  void updateTrack(std::shared_ptr<Track> track);

private:
  void clearAll();
  void addSectionHeader(QFormLayout *form, const QString &title);
  void setRowVisible(QLabel *label, QLabel *value, bool visible);

  QLabel *albumArtLabel;

  // Track Info
  QLabel *titleLabel;
  QLabel *artistLabel;
  QLabel *albumArtistLabel;
  QLabel *albumLabel;
  QLabel *yearLabel;
  QLabel *trackNumLabel;
  QLabel *discNumLabel;
  QLabel *genreLabel;
  QLabel *composerLabel;
  QLabel *bpmLabel;
  QLabel *commentLabel;
  QLabel *compilationLabel;
  QLabel *lyricsLabel;

  // Audio
  QLabel *durationLabel;
  QLabel *bitrateLabel;
  QLabel *sampleRateLabel;
  QLabel *channelsLabel;
  QLabel *codecLabel;

  // File
  QLabel *pathLabel;
  QLabel *sizeLabel;
  QLabel *modifiedLabel;

  // Row labels (for hiding empty rows)
  QLabel *titleRowLabel;
  QLabel *artistRowLabel;
  QLabel *albumArtistRowLabel;
  QLabel *albumRowLabel;
  QLabel *yearRowLabel;
  QLabel *trackNumRowLabel;
  QLabel *discNumRowLabel;
  QLabel *genreRowLabel;
  QLabel *composerRowLabel;
  QLabel *bpmRowLabel;
  QLabel *commentRowLabel;
  QLabel *compilationRowLabel;
  QLabel *lyricsRowLabel;
  QLabel *durationRowLabel;
  QLabel *bitrateRowLabel;
  QLabel *sampleRateRowLabel;
  QLabel *channelsRowLabel;
  QLabel *codecRowLabel;
  QLabel *pathRowLabel;
  QLabel *sizeRowLabel;
  QLabel *modifiedRowLabel;
};

#endif // TRACK_CONTEXT_TAB
