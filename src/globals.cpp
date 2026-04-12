#include "globals.h"

const std::string PID_FILE = "/tmp/player_cpp.pid";
const std::string File_FileInfoTxt = "~/.config/player_cpp/player_info.txt";
const std::string folderPath = "~/.config/player_cpp/dirs.txt";
const std::vector<std::string> audioNames = { ".mp3", ".wav", ".flac", ".ogg" };
AudioPlayer* player_ = nullptr;

char RESET[] = "\033[0m";
char RED[] = "\033[31m";
char GREEN[] = "\033[32m";
char YELLOW[] = "\033[33m";
char BLUE[] = "\033[34m";
char PURPLE[] = "\033[35m";
char BLUE2[] = "\033[36m";
char WHITE[] = "\033[37m";