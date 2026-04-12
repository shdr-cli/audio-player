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
    
    player->currentPosition = double(timeInfo->outputBufferDacTime);
    
    unsigned long totalFramesNeeded = framesPerBuffer;
    unsigned long framesWritten = 0;
    
    while (framesWritten < totalFramesNeeded) {
        if (!player->isPaused) {
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
    }   
    return paContinue;
}

// Конструктор с одним файлом
AudioPlayer::AudioPlayer(const std::string& filePath, bool loop) 
    : loop(loop), random(false), audioFile(nullptr), 
      audioStream(nullptr), notification(nullptr) {
    
    audioFiles.push_back(filePath);

    // Новая версия фона, закрывает вывод, потом открывает
    freopen("/dev/null","w",stderr);
    if (Pa_Initialize() != paNoError) return;
    freopen("/dev/tty","w",stderr);

    audioFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!audioFile) return;

    std::filesystem::path fPath = File_FileInfoTxt;
    if (File_FileInfoTxt[0] == '~') {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            fPath = std::filesystem::path(homeDir) / File_FileInfoTxt.substr(2); // удаление ~/
        }
    }
    // std::cout << fPath << std::endl;
    // std::ifstream file(fPath);

    // std::ofstream file(File_FileInfoTxt);
    // file << audioFiles[randomIndex];
    // file.close();

    std::ofstream file(fPath);
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
}

AudioPlayer::AudioPlayer(std::vector<std::string>& audioFiles,
        bool loop, bool random) : 
        loop(loop), random(random), audioFiles(audioFiles),
        audioFile(nullptr), audioStream(nullptr), notification(nullptr) {

    // Новая версия фона, закрывает вывод, потом открывает
    freopen("/dev/null","w",stderr);
    if (Pa_Initialize() != paNoError) return;
    freopen("/dev/tty","w",stderr);

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

void AudioPlayer::ShowNotification(const std::string& desc) {
    if (notify_init("Player")) {
        notification = notify_notification_new(
            "Player", 
            desc.c_str(),
            "audio-x-generic"
        );
        notify_notification_set_timeout(notification, 3000);
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

    std::filesystem::path fPath = File_FileInfoTxt;
    if (File_FileInfoTxt[0] == '~') {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            fPath = std::filesystem::path(homeDir) / File_FileInfoTxt.substr(2); // удаление ~/
        }
    }
    // std::cout << fPath << std::endl;
    // std::ifstream file(fPath);

    // std::ofstream file(File_FileInfoTxt);
    // file << audioFiles[randomIndex];
    // file.close();

    std::ofstream file(fPath);
    file << audioFiles[randomIndex];
    file.close();

    ShowNotification(audioFiles[randomIndex]);

    return audioFiles[randomIndex];
}

bool AudioPlayer::ToTime(double seconds) {
    if (!audioFile) return false;

    sf_count_t frame = sf_count_t(seconds*sfInfo.samplerate);
    sf_count_t result = sf_seek(audioFile, frame, SEEK_SET);

    if (result == frame) {
        currentPosition = seconds;
        return true;
    }
    return false;
}

bool AudioPlayer::ToPercentPosition(short percent) {
    if (!audioFile) return false;

    double totalTime = GetTotalTime();
    double targetTime = (percent / 100.0) * totalTime;
    return ToTime(targetTime);
}

double AudioPlayer::GetCurrentTime() {
    if (!audioFile) return 0.0;

    sf_count_t frame = sf_seek(audioFile, 0, SEEK_CUR);
    return static_cast<double>(frame) / sfInfo.samplerate;
}

// Возвращает время в секундах
double AudioPlayer::GetTotalTime() {
    if (!audioFile) return 0.0;
    
    return static_cast<double>(sfInfo.frames / sfInfo.samplerate);
}

std::string AudioPlayer::PauseUnpause(bool change) {
    if (change) isPaused = !isPaused;
    if (isPaused) {
        return "Музыка остановлена";
    } else {
        return"Пауза снята";
    }
}

bool AudioPlayer::Init() { 
    return audioFile && audioStream; 
}

bool AudioPlayer::Start() {
    if (audioStream && Pa_StartStream(audioStream) == paNoError) {
        return true;
    }
    return false;
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