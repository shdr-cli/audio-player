#include <iostream>
#include <string>
#include <csignal>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>   
#include "cpp-player.h"
#include <filesystem>
#include <algorithm>
#include "globals.h"

std::vector<std::string> audioFiles;

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void SetAudioFiles(std::vector<std::string>& audioFiles_) {
    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        for (const auto& file : std::filesystem::directory_iterator(folderPath)) {
            if (std::filesystem::is_regular_file(file)) {
                std::string ext = file.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (std::find(audioNames.begin(), audioNames.end(), ext) != audioNames.end()) {
                    audioFiles.push_back(file.path().string());
                }
            }
        }
    }
}

void PlayerPlay(std::string& filePath, bool loop) {
    SF_INFO checkInfo;
    SNDFILE* checkFile = sf_open(filePath.c_str(), SFM_READ, &checkInfo);
    if (!checkFile) {
        std::cerr << RED << "Ошибка: Файл не найден или формат не поддерживается: " << RESET << filePath << std::endl;
        exit(EXIT_FAILURE   );
    }
    sf_close(checkFile);

    std::cout << GREEN << "Играет " << RESET << filePath << std::endl;

    daemonize(); 

    AudioPlayer player(filePath, loop);
    if (player.Init() && player.Start()) {
        std::ofstream file(PID_FILE);
        file << getpid();
        file.close();

        while (Pa_IsStreamActive(player.audioStream) == 1) {
            usleep(500000); 
        }
    }
    unlink(PID_FILE.c_str());
}
void PlayerPlay(std::vector<std::string>& audioFiles, bool loop, bool random) {
    std::cout << GREEN << "Воспроизведение" << RESET << std::endl;
    daemonize(); 

    AudioPlayer player(audioFiles, loop, random);
    if (player.Init() && player.Start()) {
        std::ofstream file(PID_FILE);
        file << getpid();
        file.close();

        while (Pa_IsStreamActive(player.audioStream) == 1) {
            usleep(500000); 
        }
    }
    unlink(PID_FILE.c_str());
}

void Kill(short show_status = 0) {
    std::remove(File_FileInfoTxt.c_str());
    std::ifstream file(PID_FILE);
    if (file.is_open()) {
        pid_t pid;
        file >> pid;
        file.close();
        
        kill(pid, SIGTERM);
        unlink(PID_FILE.c_str());
        std::cout << RED << "Воспроизведение остановлено (PID " << RESET << pid << RED << ")" << RESET << std::endl;
    } else {
        if (show_status == 0) std::cout << "Нет активного плеера" << std::endl;
    }
}

void HelpUsage() {
    std::cout << "Использование: ./player " << GREEN << "play" << RESET << " " << YELLOW << "<файл.wav>" << RESET << " | " << GREEN << "stop" << RESET <<" |" << std::endl <<
    "  Другие аргументы:" << std::endl <<
    "    \t" << GREEN << "info" << RESET << " | " << GREEN << "status" << RESET << " : Вывод текущего аудио" << std::endl <<
    std::endl <<
    "    " << GREEN << "--list" << RESET << " (" << GREEN << "-l" << RESET << ") : Показывает список аудио файлов в папке ~/Music" << std::endl <<
    "    " << GREEN << "--random" << RESET << " (" << GREEN << "-r" << RESET << ") : Воспроизводит случайный аудио из папки ~/Music" << std::endl <<
    std::endl << 
    "    " << GREEN << "-loop" << RESET << " (" << GREEN << "-L" << RESET << ") : Зацикливает воспроизведение" << std::endl;
}
int main(int argc, char* argv[]) {
    bool loop, random = false;
    
    if (argc < 2) {
        HelpUsage();
        return 1;
    }

    std::string command = argv[1];

    for (int i = 1; i < argc; i++) {
        // std::cout << i << " " << argv[i] << std::endl;
        std::string arg = std::string(argv[i]);
        if (arg == "-loop" || arg == "-L") {
            loop = true;
        } else if (arg == "--random" || arg == "-r") {
            random = true;
        }
    }

    if (loop) std::cout << BLUE << "  [+] Зацикливание включено" << RESET << std::endl;

    if ((command == "--list" || command == "-l") && argc == 2) {
        try {
            SetAudioFiles(audioFiles);
        } catch (const std::filesystem::filesystem_error& error) {
            std::cerr << RED << "Ошибка: " << error.what() << RESET << std::endl;
        }

        std::cout << YELLOW << "\t\tВсего:" << audioFiles.size() <<  RESET << std::endl;
        for (const auto& file : audioFiles) {
            std::cout << file << std::endl;
        }
    } else if ((command == "--random" || command == "-r") && (argc == 2 || argc == 3)) {
        try {
            SetAudioFiles(audioFiles);
        } catch (const std::filesystem::filesystem_error& error) {
            std::cerr << RED << "Ошибка: " << error.what() << RESET << std::endl;
        }

        Kill(1);
        PlayerPlay(audioFiles, loop, random);
        
    } else if ((command == "play" && (argc == 3 || argc == 4))) {
        std::string filePath = argv[2];
        Kill(1);
        PlayerPlay(filePath, loop);
    } 
    else if (command == "stop" && argc == 2) {
        Kill();
    } else if ((command == "info" || command == "status") && argc == 2) {
        std::ifstream file(File_FileInfoTxt);
        if (!file.is_open()) {
            std::cout << RED << "Ничего не играет" << RESET << std::endl;
            exit(EXIT_FAILURE);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string filePath = buffer.str();
        filePath.pop_back();
        file.close();
        std::cout << GREEN << "Играет "  << RESET << BLUE2 << filePath << RESET << std::endl;
    } else {
        HelpUsage();
        return 1;
    }
    return 0;
}