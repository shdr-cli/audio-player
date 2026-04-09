#include "globals.h"

const std::string PID_FILE = "/tmp/cpp_player.pid";
const std::string File_FileInfoTxt = "/home/shdr/shdrScripts/player_info.txt";
const std::string folderPath = "~/.config/player-cpp/dirs.txt";
// const std::string folderPath = "/home/shdr/.config/player-cpp/dirs.txt";
const std::vector<std::string> audioNames = { ".mp3", ".wav", ".flac", ".ogg" };

char RESET[] = "\033[0m";
char RED[] = "\033[31m";
char GREEN[] = "\033[32m";
char YELLOW[] = "\033[33m";
char BLUE[] = "\033[34m";
char PURPLE[] = "\033[35m";
char BLUE2[] = "\033[36m";
char WHITE[] = "\033[37m";