#ifndef CPP_PLAYER_H
#define CPP_PLAYER_H

#include <iostream>
#include <string>
#include <sndfile.h>
#include <portaudio.h>
#include <vector>

struct _NotifyNotification;
typedef struct _NotifyNotification NotifyNotification;

class AudioPlayer {
private:
    std::string GetRandom(std::vector<std::string>& audioFiles);

public:
    bool loop;
    bool random;
    std::vector<std::string> audioFiles;

    AudioPlayer(const std::string& filePath, bool loop = false);
    AudioPlayer(std::vector<std::string>& audioFiles, bool loop = false, bool random = false);
    ~AudioPlayer();

    bool Init();
    bool Start();
    bool OpenNextRandomFile();

    void InitNotifications();
    void ShowNotification(const std::string& title, const std::string& message);
    void CleanupNotifications();
    
    NotifyNotification* notification;
    SNDFILE* audioFile;
    PaStream* audioStream;
    SF_INFO sfInfo;
};

#endif