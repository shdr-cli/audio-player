<div align="center">
  
```console
Консольный аудиоплеер с демонизацией (фон) и уведомлениями
```

![License](https://img.shields.io/badge/license-GPL--3.0-blue?style=flat-square)
![Platform](https://img.shields.io/badge/platform-Linux-1793D1?style=flat-square&logo=linux)
![Status](https://img.shields.io/badge/status-active-brightgreen?style=flat-square)

![GitHub stars](https://img.shields.io/github/stars/shdr-cli/audio-player?style=social)
![GitHub forks](https://img.shields.io/github/forks/shdr-cli/audio-player?style=social)
![GitHub watchers](https://img.shields.io/github/watchers/shdr-cli/audio-player?style=social)
![GitHub repo size](https://img.shields.io/github/repo-size/shdr-cli/audio-player?style=flat-square&color=brightgreen)
![GitHub last commit](https://img.shields.io/github/last-commit/shdr-cli/audio-player?style=flat-square&color=brightgreen)

![Debian](https://img.shields.io/badge/Debian-A81D33?style=flat-square&logo=debian&logoColor=white)
![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=flat-square&logo=ubuntu&logoColor=white)

</div>

# audio-player
### Консольный аудиоплеер на C++ с использованием PortAudio и libsndfile. Работает в фоне, поддерживает случайное воспроизведение, зацикливание и системные уведомления через libnotify.

---

## Содержание

- [Возможности](#возможности)
- [📦 Зависимости](#-зависимости)
- [Установка на Debian/Ubuntu](#установка-на-debianubuntu)
- [🚜 Сборка](#-сборка)
- [Использование](#использование)
- [📂 Структура проекта](#-структура-проекта)
- [🔧 Конфигурация](#-конфигурация)
- [Бонус](#бонус)
- [Roadmap](#roadmap)

---

# Возможности

| Функция | Описание |
| :--- | :---: |
|Воспроизведение | WAV, MP3, FLAC, OGG (через libsndfile)|
|Фон| Плеер уходит в фон, не блокируя терминал|
|Зацикливание| -loop / -L - бесконечное повторение|
|Случайный трек	| --random / -r - случайный трек из ~/.config/player-cpp/dirs.txt (содержит пути к директориям с аудио)|
|Уведомления | Через libnotify (начало, конец, смена трека)|
|Плейлист | Автоматически собирает все аудио из файла dirs.txt (в конфиге player-cpp)|
|Добавление папки с аудио|Путь с аудио добавляется в файл dirs.txt|
|Удаление папки с аудио|Путь с аудио удаляется из файла dirs.txt|
|Управление | stop, info, status|
|Цветной вывод | Информативные сообщения в терминале|

# 📦 Зависимости
| Библиотека | Назначение |
| :--- | :---: |
|PortAudio|Вывод звука|
|Libsndfile|Чтение аудиофайлов (WAV, FLAC, OGG)|
|Libnotify|Системные уведомления|
|glib2|Для libnotify|

# Установка на Debian/Ubuntu
```bash
apt update && \
apt install libnotify-dev libglib2.0-dev \
    libsndfile1 libsndfile1-dev \
    libportaudio2 portaudio19-dev
```

# 🚜 Сборка
```bash
git clone https://github.com/shdr-cli/audio-player.git
cd audio-player
cmake -B build
cmake --build build
```

# Использование
## Основные команды
```bash
# Воспроизвести файл
./player play /path/to/song.wav

# Воспроизвести с зацикливанием
./player play /path/to/song.wav -loop

# Перемотать аудио на 70%
./player at 70

# Добавление директории
./player add ~/Music

# Удаление директории
./player remove ~/Music

# Случайный трек (трек берётся из ~/.config/player-cpp/dirs.txt)
./player -random

# Случайные треки с зацикливанием (каждый следующий трек случайный)
./player -random -loop

# Показать список всех аудио
./player list

# Остановить воспроизведение
./player stop

# Показать, что сейчас играет
./player info
./player status
```
### Подробно смотрите --help

# 📂 Структура проекта

```
├── CMakeLists.txt
├── LICENSE
├── README.md
├── convert
│   ├── flac-mp3.sh
│   ├── m4a-mp3.sh
│   └── wav-mp3.sh
├── include
│   ├── pa_asio.h
│   ├── pa_jack.h
│   ├── pa_linux_alsa.h
│   ├── pa_mac_core.h
│   ├── pa_win_ds.h
│   ├── pa_win_wasapi.h
│   ├── pa_win_waveformat.h
│   ├── pa_win_wdmks.h
│   ├── pa_win_wmme.h
│   └── portaudio.h
├── install-depend
├── run
└── src
    ├── args.cpp
    ├── args.h
    ├── cpp-player.cpp
    ├── cpp-player.h
    ├── globals.cpp
    ├── globals.h
    ├── main.cpp
    ├── signals.cpp
    └── signals.h
```

# 🔧 Конфигурация
|Переменная|Значение по умалчанию|Описание|
|:---|:---|:---:|
|folderPath|~/.config/player_cpp/dirs.txt|Файл с путями директорий аудио|
|PID_FILE|/tmp/player_cpp.pid|Файл с PID фонового скрипта|
|File_FileInfoTxt|~/.config/player_cpp/player_info.txt|Файл с путём текущего трека|
|audioNames|.mp3, .wav, .flac, .ogg|Поддерживаемые форматы|

## Уведомления приходят пока для аргумента random

# Бонус
в папке convert есть скрипты для конвертации аудио из m4a, flac, wav в mp3

# Roadmap

### v0.1.0 - Базовая функциональность

- [x] Воспроизведение аудиофайлов (`WAV`, `MP3`, `FLAC`, `OGG`)
- [x] Работа в фоне
- [x] Управление через CLI (`play`, `stop`, `info`)
- [x] Зацикливание (`-loop`)
- [x] Случайное воспроизведение (`-random`)
- [x] Системные уведомления (libnotify)
- [x] Цветной вывод в терминале
- [x] Пауза / Возобновление (`player pause/player unpause`)
- [ ] Следующий трек / Предыдущий трек (`player next`, `player orev`)
- [x] Перемотка (в процентах) (`player at`)
- [ ] Отображение прогресс бара

---

<div align="center">

# Если проект понравился — поставь ⭐
### Это мотивирует продолжать разработку

![GitHub issues](https://img.shields.io/github/issues/shdr-cli/audio-player?style=flat-square&color=red)
![GitHub pull requests](https://img.shields.io/github/issues-pr/shdr-cli/audio-player?style=flat-square&color=blue)
![GitHub contributors](https://img.shields.io/github/contributors/shdr-cli/audio-player?style=flat-square)

</div>