#include "library_model.hpp"
#include <algorithm>
#include <QDebug>

// Utility function to extract album artwork from a track
QPixmap extractAlbumArtwork(const std::string& filePath) {
    QPixmap pixmap;
    
    qDebug() << "Extracting artwork from:" << QString::fromStdString(filePath);
    
    // Try to extract artwork based on file type
    std::string ext = filePath.substr(filePath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    qDebug() << "File extension:" << QString::fromStdString(ext);
    
    if (ext == "mp3") {
        // MP3 with ID3v2 tags
        TagLib::MPEG::File file(filePath.c_str());
        if (file.isValid() && file.ID3v2Tag()) {
            TagLib::ID3v2::Tag *tag = file.ID3v2Tag();
            TagLib::ID3v2::FrameList pictures = tag->frameList("APIC");
            
            if (!pictures.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame *picture = 
                    static_cast<TagLib::ID3v2::AttachedPictureFrame *>(pictures.front());
                if (picture && !picture->picture().isEmpty()) {
                    QByteArray data(picture->picture().data(), picture->picture().size());
                    pixmap.loadFromData(data);
                    qDebug() << "Found MP3 artwork, size:" << pixmap.size();
                } else {
                    qDebug() << "No MP3 artwork found in APIC frame";
                }
            } else {
                qDebug() << "No APIC frames found in MP3 file";
            }
        }
    } else if (ext == "flac") {
        // FLAC files
        TagLib::FLAC::File file(filePath.c_str());
        if (file.isValid()) {
            TagLib::List<TagLib::FLAC::Picture *> pictures = file.pictureList();
            if (!pictures.isEmpty()) {
                TagLib::FLAC::Picture *picture = pictures.front();
                if (picture && !picture->data().isEmpty()) {
                    QByteArray data(picture->data().data(), picture->data().size());
                    pixmap.loadFromData(data);
                }
            }
        }
    } else if (ext == "m4a" || ext == "mp4") {
        // MP4/M4A files
        TagLib::MP4::File file(filePath.c_str());
        if (file.isValid() && file.tag()) {
            TagLib::MP4::Tag *tag = file.tag();
            TagLib::MP4::ItemMap items = tag->itemMap();
            
            if (items.contains("covr")) {
                TagLib::MP4::Item coverItem = items["covr"];
                TagLib::MP4::CoverArtList coverArtList = coverItem.toCoverArtList();
                if (!coverArtList.isEmpty()) {
                    TagLib::MP4::CoverArt coverArt = coverArtList.front();
                    QByteArray data(coverArt.data().data(), coverArt.data().size());
                    pixmap.loadFromData(data);
                }
            }
        }
    } else if (ext == "ogg") {
        // OGG Vorbis files
        TagLib::Ogg::Vorbis::File file(filePath.c_str());
        if (file.isValid()) {
            TagLib::Ogg::XiphComment *tag = file.tag();
            if (tag) {
                TagLib::List<TagLib::FLAC::Picture *> pictures = tag->pictureList();
                if (!pictures.isEmpty()) {
                    TagLib::FLAC::Picture *picture = pictures.front();
                    if (picture && !picture->data().isEmpty()) {
                        QByteArray data(picture->data().data(), picture->data().size());
                        pixmap.loadFromData(data);
                    }
                }
            }
        }
    }
    
    if (pixmap.isNull()) {
        qDebug() << "No artwork extracted from file";
    } else {
        qDebug() << "Successfully extracted artwork, size:" << pixmap.size();
    }
    
    return pixmap;
}

// Implementation of AlbumItem::updateAlbumArtwork
void AlbumItem::updateAlbumArtwork() {
    // Get the first track child to extract artwork
    if (rowCount() > 0) {
        QStandardItem *firstTrackItem = child(0);
        if (firstTrackItem) {
            LibraryItem *trackLibraryItem = static_cast<LibraryItem *>(firstTrackItem);
            if (trackLibraryItem && trackLibraryItem->getType() == LibraryItem::Type::Track) {
                TrackItem *trackItem = static_cast<TrackItem *>(trackLibraryItem);
                QString trackPath = trackItem->getFilePath();
                
                if (!trackPath.isEmpty()) {
                    QPixmap artwork = extractAlbumArtwork(trackPath.toStdString());
                    if (!artwork.isNull()) {
                        // Scale the artwork to a reasonable size for the icon
                        QPixmap scaledArtwork = artwork.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        setIcon(QIcon(scaledArtwork));
                        qDebug() << "Updated album artwork for:" << text();
                        return;
                    }
                }
            }
        }
    }
    
    // If no artwork found, keep the default icon
    setDefaultAlbumIcon();
    qDebug() << "No artwork found for album:" << text();
}

// Implementation of AlbumItem::setDefaultAlbumIcon
void AlbumItem::setDefaultAlbumIcon() {
    // Try system theme icons first, then fallback to a simple generated icon
    QIcon defaultIcon = QIcon::fromTheme("media-optical-audio", 
                                         QIcon::fromTheme("audio-x-generic", 
                                                         QIcon::fromTheme("folder-music")));
    
    if (defaultIcon.isNull()) {
        // Create a simple colored square as fallback
        QPixmap fallbackPixmap(64, 64);
        fallbackPixmap.fill(QColor(100, 150, 200)); // Light blue color
        defaultIcon = QIcon(fallbackPixmap);
    }
    
    setIcon(defaultIcon);
}

// Implementation of AlbumItem::updateDisplayText
void AlbumItem::updateDisplayText() {
    if (albumPtr) {
        QString newText = createDisplayText(albumPtr);
        setText(newText);
    }
}