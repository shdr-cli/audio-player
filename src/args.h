#ifndef ARGS_H
#define ARGS_H

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "globals.h"
#include "signals.h"

// Добавление в массив списка аудио (для random)
void SetAudioFiles(std::vector<std::string>& audioFiles_);
// Нормализация формата времени (длительность аудио возвращается в секундах, возвращается результат с нормальным форматом минуты:секунды)
std::string NormalDuration(double allseconds);
// Проигрывание одного аудио файла
void PlayerPlay(std::string& filePath, bool loop);
// Проигрывание случайного аудио
void PlayerPlay(std::vector<std::string>& audioFiles, bool loop, bool random);
// Помощь в использовании аргументов
void HelpUsage();
// Вывод списка добавленных директорий с аудио (выводятся директории и аудио)
void listArg(bool all = false);
// Получение случайного аудио для проигрывания
void randomArg(bool loop, bool random);
// Получение информации о аудио (что играет и длительность)
void infoArg();
// Добавление директории с аудио
void addArg(const std::string& path);
// Удаление директории с аудио
void removeArg(const std::string& path);
// Перемотка аудио в процентах
void ToTime(char* argv[]);

#endif