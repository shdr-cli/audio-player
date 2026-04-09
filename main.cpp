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
#include <iomanip>

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
    std::filesystem::path fPath = folderPath;
    if (folderPath[0] == '~') {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            fPath = std::filesystem::path(homeDir) / folderPath.substr(2); // удаление ~/
        }
    }
    
    if (!std::filesystem::exists(fPath)) {
        std::cout << "Путь '" << fPath << "' не найден" << std::endl;
        return;
    }
    
    std::vector<std::string> paths;
    std::ifstream inFile(fPath);
    std::string line;
    
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            paths.push_back(line);
        }
    }
    inFile.close();

    for (const auto& path : paths) {
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            for (const auto& file : std::filesystem::directory_iterator(path)) {
                if (std::filesystem::is_regular_file(file)) {
                    std::string ext = file.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    if (std::find(audioNames.begin(), audioNames.end(), ext) != audioNames.end()) {
                        audioFiles_.push_back(file.path().string());
                    }
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
    struct print {
        static void command(const char* name, const char* desc, const short width = 15) {
            std::cout << "    " << GREEN << std::left << std::setw(width) << name << RESET << desc << "\n";
        }
        
        static void option(const char* flags, const char* desc, const short width = 15) {
            std::cout << "    " << GREEN << std::left << std::setw(width) << flags << RESET << desc << "\n";
        }
    };
    
    std::cout << "\nИспользование:\n";
    std::cout << "    ./player " << GREEN << "play" << RESET << " " << YELLOW << "<файл.wav>" << RESET 
              << " | " << GREEN << "stop" << RESET 
              << " | " << GREEN << "info" << RESET 
              << " | " << GREEN << "status" << RESET << "\n\n";
    
    std::cout << "Команды:\n";
    print::command("play <файл>", "Воспроизвести указанный WAV-файл", 19);
    print::command("list",   "Показать список аудио файлов");
    print::command("stop",        "Остановить воспроизведение");
    print::command("add <путь>",         "Добавить директорию", 19);
    print::command("remove <путь>",      "Удалить директорию", 19);
    print::command("info",        "Информация о текущем треке");
    print::command("status",      "Статус плеера");
    
    std::cout << "\nОпции:\n";
    print::option("-r, -random", "Случайное воспроизведение из ~/Music");
    print::option("-l, -loop",   "Зациклить воспроизведение");
    
    std::cout << "\nПримеры:\n";
    std::cout << "    ./player play song.wav\n";
    std::cout << "    ./player -random -loop\n";
    std::cout << "    ./player stop\n\n";
}

void listArg() {
    try {
        SetAudioFiles(audioFiles);
    } catch (const std::filesystem::filesystem_error& error) {
        std::cerr << RED << "Ошибка: " << error.what() << RESET << std::endl;
    }

    std::filesystem::path fPath = folderPath;
    if (folderPath[0] == '~') {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            fPath = std::filesystem::path(homeDir) / folderPath.substr(2); // удаление ~/
        }
    }

    std::ifstream file(fPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << "\t\t" << YELLOW << "Плейлисты:" << RESET << std::endl <<
        buffer.str() << std::endl;

    std::cout << YELLOW << "\t\tВсего: " << audioFiles.size() <<  RESET << std::endl;
    for (const auto& file : audioFiles) {
        std::cout << file << std::endl;
    }
}

void randomArg(bool loop, bool random) {
    try {
        SetAudioFiles(audioFiles);
    } catch (const std::filesystem::filesystem_error& error) {
        std::cerr << RED << "Ошибка: " << error.what() << RESET << std::endl;
    }

    Kill(1);
    PlayerPlay(audioFiles, loop, random);
}

void infoArg() {
    std::ifstream file(File_FileInfoTxt);
    if (!file.is_open()) {
        std::cout << RED << "Ничего не играет" << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string filePath = buffer.str();
    // filePath.pop_back();
    file.close();
    std::cout << GREEN << "Играет "  << RESET << BLUE2 << filePath << RESET << std::endl;
}

void addArg(const std::string& path) {
    std::filesystem::path configDir;
    const char* homeDir = getenv("HOME");
    if (homeDir) {
        configDir = std::filesystem::path(homeDir) / ".config" / "player-cpp";
    }
    
    std::filesystem::path configFile = configDir / "dirs.txt";
    
    if (!std::filesystem::exists(path)) {
        std::cout << "Путь '" << path << "' не найден" << std::endl;
        return;
    }
    
    if (!std::filesystem::is_directory(path)) {
        std::cout << "Путь '" << path << "' не является директорией" << std::endl;
        return;
    }

    std::error_code ec;
    std::filesystem::create_directories(configDir, ec);
    
    std::ofstream file(configFile, std::ios::app);
    if (file.is_open()) {
        file << path << "\n";
        std::cout << "Путь '" << path << "' добавлен" << std::endl;
    } else {
        std::cerr << "Ошибка: не удалось открыть " << configFile << std::endl;
        exit(EXIT_FAILURE);
    }
}

void removeArg(const std::string& path) {
    std::filesystem::path configDir;
    const char* homeDir = getenv("HOME");
    if (homeDir) {
        configDir = std::filesystem::path(homeDir) / ".config" / "player-cpp";
    }
    
    std::filesystem::path configFile = configDir / "dirs.txt";
    
    if (!std::filesystem::exists(configFile)) {
        std::cout << "Путь '" << path << "' не найден" << std::endl;
        return;
    }
    
    std::vector<std::string> paths;
    std::ifstream inFile(configFile);
    std::string line;
    
    while (std::getline(inFile, line)) {
        if (line != path) paths.push_back(line);
    }
    inFile.close();
    
    std::ofstream outFile(configFile);
    for (const auto& path : paths) {
        outFile << path << "\n";
    }
    
    std::cout << "Путь '" << path << "' удалён" << std::endl;
}

int main(int argc, char* argv[]) {
    bool loop, random = false;
    
    if (argc < 2) {
        HelpUsage();
        return 1;
    }

    std::string command = argv[1];

    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg == "-loop" || arg == "-l") {
            loop = true;
        } else if (arg == "-random" || arg == "-r") {
            random = true;
        }
    }

    if (loop) std::cout << BLUE << "  [+] Зацикливание включено" << RESET << std::endl;

    if ((command == "list") && argc == 2) { listArg(); }
    
    else if ((command == "--random" || command == "-r") && (argc == 2 || argc == 3)) { randomArg(loop, random); }
    
    else if ((command == "play" && (argc == 3 || argc == 4))) {
        std::string filePath = argv[2];
        Kill(1);
        PlayerPlay(filePath, loop);
    }

    else if (command == "stop" && argc == 2) {
        Kill();
    }
    
    else if ((command == "info" || command == "status") && argc == 2) { infoArg(); }

    else if (command == "add" && argc == 3) {
        std::string path = argv[2];
        addArg(path);
    }
    
    else if (command == "remove" && argc == 3) {
        std::string path = argv[2];
        removeArg(path);
    }

    else {
        HelpUsage();
        return 1;
    }
    return 0;
}