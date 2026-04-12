#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>
#include <cpp-player.h>

extern const std::string PID_FILE;
extern const std::string File_FileInfoTxt;
extern const std::string folderPath;
extern const std::vector<std::string> audioNames;

extern AudioPlayer* player_;

enum class SignalType : int {
    PEREMOTKA = 1,
    TOTAL_TIME = 2,
    PAUSE_UNPAUSE = 3
};

// Цвета для терминала
extern char RESET[];
extern char RED[];
extern char GREEN[];
extern char YELLOW[];
extern char BLUE[];
extern char PURPLE[];
extern char BLUE2[];
extern char WHITE[];

#endif