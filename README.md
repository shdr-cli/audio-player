<div align="center">
  
```console
Консольный аудиоплеер с демонизацией (фон) и уведомлениями
```

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
- [📂 Структура проекта](#-структура-проекта)
- [🔧 Конфигурация](#-конфигурация)
- [Бонус](#бонус)

---

# Возможности

| Функция | Описание |
| :--- | :---: |
|Воспроизведение | WAV, MP3, FLAC, OGG (через libsndfile)|
|Фон| Плеер уходит в фон, не блокируя терминал|
|Зацикливание| -loop / -L - бесконечное повторение|
|Случайный трек	| --random / -r - играть случайное из ~/Music|
|Уведомления | Через libnotify (начало, конец, смена трека)|
|Плейлист | Автоматически собирает все аудио из папки Music|
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
```console
apt update && \
apt install libnotify-dev libglib2.0-dev libsndfile1 libsndfile1-dev libportaudio2 portaudio19-dev
```

# 🚜 Сборка
```console
git clone https://github.com/shdr-cli/audio-player.git
cd audio-player
cmake -B build
cmake --build build
```

# Использование
## Основные команды
```console
# Воспроизвести файл
./player play /path/to/song.wav

# Воспроизвести с зацикливанием
./player play /path/to/song.wav -loop

# Случайный трек (трек берётся из ~/Music)
./player --random

# Случайные треки с зацикливанием (каждый следующий трек случайный)
./player --random -loop

# Показать список всех аудио в ~/Music
./player --list

# Остановить воспроизведение
./player stop

# Показать, что сейчас играет
./player info
./player status
```
### Подробно смотрите --help

# 📂 Структура проекта

```
audio-player
├── CMakeLists.txt
├── LICENSE
├── README.md
├── cpp-player.cpp
├── cpp-player.h
├── demo
│   ├── YEUZ – LOW.mp3
│   └── YEUZ – RAW.mp3
├── globals.cpp
├── globals.h
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
├── m4a-mp3.sh
├── main.cpp
├── player-cpp
└── run
```

# 🔧 Конфигурация
|Переменная|Значение по умалчанию|Описание|
|:---|:---|:---:|
|folderPath|/home/shdr/Music|Папка с музыкой|
|PID_FILE|/tmp/cpp_player.pid|Файл с PID фонового скрипта|
|File_FileInfoTxt|/home/shdr/shdrScripts/player_info.txt|Файл с путём текущего трека|
|audioNames|.mp3, .wav, .flac, .ogg|Поддерживаемые форматы|

# Бонус
в папке convert есть скрипты для конвертации аудио из m4a, flac, wav в mp3