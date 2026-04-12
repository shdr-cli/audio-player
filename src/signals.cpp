#include "signals.h"

void Kill(short show_status) {
    std::filesystem::path fPath = File_FileInfoTxt;
    if (File_FileInfoTxt[0] == '~') {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            fPath = std::filesystem::path(homeDir) / File_FileInfoTxt.substr(2); // удаление ~/
        }
    }

    std::remove(fPath.c_str());
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

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    // Старая версия для фона, полностью отключение вывода

    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);
}

void signalHandler(int signalNum) {
    if (!player_) return;

    std::ifstream sigtype("/tmp/player_sigtype.tmp");
    if (!sigtype.is_open()) return;

    std::string signalType;
    // sigtype >> signalType;
    std::string value;
    int cam;
    std::getline(sigtype, signalType);
    std::getline(sigtype, value);

    switch(atoi(signalType.c_str())) {
        case static_cast<int>(SignalType::PEREMOTKA): {
            player_->ToPercentPosition(static_cast<short>(std::stoi(value)));
            break;
        }
        case static_cast<int>(SignalType::TOTAL_TIME): {
            std::cout << player_->PauseUnpause(false) << std::endl;
            std::cout << "Длительность: " << NormalDuration(player_->GetCurrentTime()) <<
            "/" << NormalDuration(player_->GetTotalTime()) << std::endl;
            break;
        }
        case static_cast<int>(SignalType::PAUSE_UNPAUSE): {
            std::cout << player_->PauseUnpause() << std::endl;
            break;
        }
    }
    std::remove("/tmp/player_sigtype.tmp");
}

void setSignal(int type, std::string value) {
    std::ofstream file("/tmp/player_sigtype.tmp");
    if (file.is_open()) {
        file << type << std::endl << value;
        file.close();
    }

    std::ifstream pidFile(PID_FILE);
    if (!pidFile.is_open()) return;
    
    pid_t pid;
    pidFile >> pid;
    pidFile.close();
    kill(pid, SIGUSR1);
}