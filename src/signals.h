#ifndef SIGNALS_H
#define SIGNALS_H

#include <iostream>
#include <fstream>
#include "globals.h"
#include "args.h"
#include <unistd.h>
#include <sys/stat.h>
#include <csignal>

// Остановка плеера и проверка статуса (запущен или нет)
void Kill(short show_status = 0);
// Уход программы в фон
void daemonize();
// Обработка сигналов (для перемотки и длительности аудио)
void signalHandler(int signalNul);
// Установка сигнала
void setSignal(int type, std::string value = "none");

#endif