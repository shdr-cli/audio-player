#include "args.h"

std::vector<std::string> audioFiles;

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

std::string NormalDuration(double allseconds) {
    int minutes = static_cast<int>(allseconds) / 60;
    int seconds = static_cast<int>(allseconds) % 60;
    
    std::string result = std::to_string(minutes) + ":";
    
    if (seconds < 10) {
        result += "0" + std::to_string(seconds);
    } else {
        result += std::to_string(seconds);
    }
    
    return result;
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

    signal(SIGUSR1, signalHandler);

    AudioPlayer player(filePath, loop);
    player_ = &player;
    if (player.Init() && player.Start()) {
        std::ofstream file(PID_FILE);
        file << getpid();
        file.close();

        while (Pa_IsStreamActive(player.audioStream) == 1) {
            usleep(500000); 
        }
    }
    player_ = nullptr;
    unlink(PID_FILE.c_str());
}

void PlayerPlay(std::vector<std::string>& audioFiles, bool loop, bool random) {
    // std::cout << GREEN << "Воспроизведение" << RESET << std::endl;
    daemonize(); 

    signal(SIGUSR1, signalHandler);

    AudioPlayer player(audioFiles, loop, random);
    player_ = &player;
    if (player.Init() && player.Start()) {
        std::ofstream file(PID_FILE);
        file << getpid();
        file.close();

        while (Pa_IsStreamActive(player.audioStream) == 1) {
            usleep(500000); 
        }
    }
    player_ = nullptr;
    unlink(PID_FILE.c_str());
}

void HelpUsage() {
    struct print {
        static void command(const char* name, const char* desc, const short width = 30) {
            std::cout << "    " << GREEN << std::left << std::setw(width) << name << RESET << desc << "\n";
        }
        
        static void option(const char* flags, const char* desc, const short width = 30) {
            std::cout << "    " << GREEN << std::left << std::setw(width) << flags << RESET << desc << "\n";
        }
    };
    
    std::cout << "Использование:\n";
    std::cout << "    ./player " << GREEN << "play" << RESET << " " << YELLOW << "<файл.wav>" << RESET 
              << " | " << GREEN << "stop" << RESET 
              << " | " << GREEN << "info" << RESET 
              << " | " << GREEN << "status" << RESET << "\n\n";
    
    std::cout << "Команды:\n";
    print::command("play <файл>", "Воспроизвести указанный WAV-файл", 34);
    print::command("list",   "Показать список аудио файлов");
    print::command("stop",        "Остановить воспроизведение");
    print::command("add <путь>",         "Добавить директорию", 34);
    print::command("remove <путь>",      "Удалить директорию", 34);
    print::command("at <значение от 0-100>",      "Перемотка аудио (в процентах)", 40);
    print::command("info",        "Информация о текущем треке");
    print::command("status",      "Статус плеера");
    
    std::cout << "\nОпции:\n";
    print::option("list -s", "Список найденных аудио");
    print::option("-r, -random", "Случайное воспроизведение");
    print::option("-l, -loop",   "Зациклить воспроизведение");
    
    std::cout << "\nПримеры:\n";
    std::cout << "    ./player play song.wav\n";
    std::cout << "    ./player -random -loop\n";
    std::cout << "    ./player stop\n\n";
}

void listArg(bool all) {
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
        buffer.str();

    if (all) {
        std::cout << YELLOW << "\n\t\tВсего: " << audioFiles.size() <<  RESET << std::endl;
        for (const auto& file : audioFiles) {
            std::cout << file << std::endl;
        }
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

    // Отправка сиганала для получения информации о длительности аудио
    setSignal(static_cast<int>(SignalType::TOTAL_TIME));
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

void ToTime(char* argv[]) {
    std::ifstream pidFile(PID_FILE);
    if (!pidFile.is_open()) {
        std::cout << RED << "Нет активного плеера" << RESET << std::endl;
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    pidFile >> pid;
    pidFile.close();

    short time = atoi(argv[2]);

    if (time < 0 || time > 100) {
        std::cout << RED << "Перемотать аудио можно от 0% до 100%" << RESET << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ofstream seekFile("/tmp/player_time.tmp");
    seekFile << time;
    seekFile.close();
    
    setSignal(static_cast<int>(SignalType::PEREMOTKA), std::to_string(time));

    std::cout << GREEN << "Перемотка на " << time << "%" << RESET << std::endl;

    // if (kill(pid, SIGUSR1) == 0) {
    //     std::cout << GREEN << "Перемотка на " << time << "%" << RESET << std::endl;
    // } else {
    //     std::cout << RED << "Ошибка" << RESET << std::endl;
    // }
}