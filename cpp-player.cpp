#include "cpp-player.h"
#include <unistd.h>
#include <csignal>
#include <fstream>
#include <random>
#include <libnotify/notify.h>
#include "globals.h"

int AudioCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData) {
    
    AudioPlayer* player = (AudioPlayer*)userData;
    float* out = (float*)outputBuffer;
    
    unsigned long totalFramesNeeded = framesPerBuffer;
    unsigned long framesWritten = 0;
    
    while (framesWritten < totalFramesNeeded) {
        sf_count_t readCount = sf_readf_float(player->audioFile, 
            out + (framesWritten * player->sfInfo.channels), 
            totalFramesNeeded - framesWritten);
        
        framesWritten += readCount;

        // Если аудио закончилось
        if (readCount == 0) {
            if (player->loop) { // Зацикливание
                if (player->random) { // Случайное воспроизведение аудио
                    if (!player->OpenNextRandomFile()) {
                        for (unsigned long i = framesWritten * player->sfInfo.channels; 
                             i < totalFramesNeeded * player->sfInfo.channels; i++) {
                            out[i] = 0.0f;
                        }
                        return paComplete;
                    }
                    // Показываем уведомление о смене трека
                    // player->ShowNotification("Следующий трек", "Воспроизведение следующего трека");
                } else {
                    sf_seek(player->audioFile, 0, SEEK_SET);
                }
            } else {
                for (unsigned long i = framesWritten * player->sfInfo.channels; 
                     i < totalFramesNeeded * player->sfInfo.channels; i++) {
                    out[i] = 0.0f;
                }
                // player->ShowNotification("Воспроизведение завершено", "Конец трека");
                return paComplete;
            }
        }
    }   
    return paContinue;
}

// Конструктор с одним файлом
AudioPlayer::AudioPlayer(const std::string& filePath, bool loop) 
    : loop(loop), random(false), audioFile(nullptr), 
      audioStream(nullptr), notification(nullptr) {
    
    audioFiles.push_back(filePath);
    
    if (Pa_Initialize() != paNoError) return;

    audioFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!audioFile) return;

    std::ofstream file(File_FileInfoTxt);
    file << filePath;
    file.close();

    PaStreamParameters output;
    output.device = Pa_GetDefaultOutputDevice();
    output.channelCount = sfInfo.channels;
    output.sampleFormat = paFloat32;
    output.suggestedLatency = Pa_GetDeviceInfo(output.device)->defaultLowOutputLatency;
    output.hostApiSpecificStreamInfo = nullptr;

    Pa_OpenStream(&audioStream, nullptr, &output, sfInfo.samplerate,
                  1024*4, paClipOff, AudioCallback, this);
    
    InitNotifications();
}

// Конструктор с плейлистом
AudioPlayer::AudioPlayer(std::vector<std::string>& audioFiles,
        bool loop, bool random) : 
        loop(loop), random(random), audioFiles(audioFiles),
        audioFile(nullptr), audioStream(nullptr), notification(nullptr) {

    if (Pa_Initialize() != paNoError) return;

    std::string filePath = GetRandom(audioFiles);
    
    audioFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!audioFile) return;

    PaStreamParameters output;
    output.device = Pa_GetDefaultOutputDevice();
    output.channelCount = sfInfo.channels;
    output.sampleFormat = paFloat32;
    output.suggestedLatency = Pa_GetDeviceInfo(output.device)->defaultLowOutputLatency;
    output.hostApiSpecificStreamInfo = nullptr;

    Pa_OpenStream(&audioStream, nullptr, &output, sfInfo.samplerate,
                  1024*4, paClipOff, AudioCallback, this);
    
    InitNotifications();
}

AudioPlayer::~AudioPlayer() {
    CleanupNotifications();
    
    if (audioStream) {
        Pa_StopStream(audioStream);
        Pa_CloseStream(audioStream);
    }
    if (audioFile) sf_close(audioFile);
    Pa_Terminate();
}

bool AudioPlayer::OpenNextRandomFile() {
    if (audioFiles.empty()) return false;
    
    if (audioFile) {
        sf_close(audioFile);
        audioFile = nullptr;
    }
    
    std::string filePath = GetRandom(audioFiles);
    audioFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
        
    return audioFile != nullptr;
}

void AudioPlayer::InitNotifications() {
    if (notify_init("Player")) {
        notification = notify_notification_new(
            "Player", 
            "Плеер инициализирован",
            "audio-x-generic"
        );
        notify_notification_set_timeout(notification, 3000);
        notify_notification_show(notification, nullptr);
    }
}

void AudioPlayer::ShowNotification(const std::string& title, const std::string& message) {
    if (notification) {
        notify_notification_update(notification, title.c_str(), message.c_str(), "audio-x-generic");
        notify_notification_show(notification, nullptr);
    }
}

void AudioPlayer::CleanupNotifications() {
    if (notification) {
        g_object_unref(G_OBJECT(notification));
        notification = nullptr;
    }
    notify_uninit();
}

std::string AudioPlayer::GetRandom(std::vector<std::string>& audioFiles) {
    if (audioFiles.empty()) return "";
    
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    size_t randomIndex = gen() % audioFiles.size();

    std::ofstream file(File_FileInfoTxt);
    file << audioFiles[randomIndex];
    file.close();

    return audioFiles[randomIndex];
}

bool AudioPlayer::Init() { 
    return audioFile && audioStream; 
}

bool AudioPlayer::Start() {
    if (audioStream && Pa_StartStream(audioStream) == paNoError) {
        ShowNotification("Воспроизведение", "Начало воспроизведения");
        return true;
    }
    return false;
}