#ifndef TRACK
#define TRACK


#include <string>

class Track {
public:
    explicit Track(std::string fullPath){
        this->fullPath = fullPath;
    };
    std::string fullPath;
    std::string comment;
    std::string album;
    std::string artist;
    std::string title;
    std::string genre;
    int year;
    int track;
};


#endif // !TRACK
