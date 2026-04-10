#include "globals.h"
#include "args.h"

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
            for (int j = 1; j < argc; j++) {
                std::string arg = std::string(argv[j]);
                if (arg == "play" || arg == "-r" || arg == "-random") loop = true;
                break;
            }
        } else if (arg == "-random" || arg == "-r") {
            random = true;
        }
    }

    if (loop) std::cout << BLUE << "  [+] Зацикливание включено" << RESET << std::endl;

    if (command == "list" && (argc == 2 || argc == 3)) {
        if (argv[2] && std::string(argv[2]) == "-s") listArg(true);
        else listArg();
    }
    
    else if ((command == "--random" || command == "-r") && (argc == 2 || argc == 3)) { randomArg(loop, random); }
    
    else if ((command == "play" && (argc == 3 || argc == 4))) {
        std::string filePath = argv[2];
        Kill(1);
        PlayerPlay(filePath, loop);
    }

    else if (command == "stop" && argc == 2) { Kill(); }
    
    else if ((command == "info" || command == "status") && argc == 2) { infoArg(); }

    else if (command == "add" && argc == 3) {
        std::string path = argv[2];
        addArg(path);
    }
    
    else if (command == "remove" && argc == 3) {
        std::string path = argv[2];
        removeArg(path);
    }

    else if (command == "at" && argc == 3) {
        ToTime(argv);
    }

    else {
        HelpUsage();
        return 1;
    }
    return 0;
}