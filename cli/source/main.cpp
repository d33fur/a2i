#include "raylib.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <a2i/spectrogram.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#include "config.hpp"
#include "parser.hpp"
#include "version.hpp"
#include "nlohmann/json.hpp"
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef _WIN32
int getch() {
    return _getch();
}

int kbhit() {
    return _kbhit();
}
#else
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

typedef struct {
  std::vector<cv::Mat> prev;
  cv::Mat img;
  cv::Mat cur_img;
  cv::Mat grid;

} Window;

typedef struct {
    float left;
    float right;
} Frame;

Window w;

a2i::Spectrogram g;

std::pair<int, int> WINDOW;
unsigned int FRAME_SIZE;
int multiplier;
bool SHOW = false;
bool DEBUG_MODE;
bool ONLY_AUDIO;
bool MIC_MODE;

auto config = Config();

std::atomic<bool> PAUSE(true);
std::atomic<bool> RUNNING(true);



void callback(void *bufferData, unsigned int frames) {
    if (frames < 512) return;

    Frame *fs = static_cast<Frame*>(bufferData);

    if (g.in.size() == FRAME_SIZE) {
        g.in.erase(g.in.begin(), g.in.begin() + 512);
    }

    for (size_t i = 0; i < 512; ++i) {
        g.in.push_back((fs[i].left + fs[i].right) / 2);
    }

    if (g.in.size() == FRAME_SIZE) {
        g.addWindow();
        g.fft();
        g.normalize(multiplier);
        SHOW = true;
    } else {
        SHOW = false;
    }
}

void handleConsoleInput(Music &music, std::atomic<bool> &RUNNING) {
    while (RUNNING) {
        if (kbhit()) {
            int key = getch();
            if (key == ' ') {
                if (PAUSE)
                    PauseMusicStream(music);
                else
                    ResumeMusicStream(music);
                PAUSE = !PAUSE;
            } else if (key == 'q' || key == 'Q') {
                RUNNING = false;
                break;
            } else if (key == 'd' || key == 'D') {
                float current_time = GetMusicTimePlayed(music);
                SeekMusicStream(music, current_time + 5.0f);
            } else if (key == 'a' || key == 'A') {
                float current_time = GetMusicTimePlayed(music);
                SeekMusicStream(music, current_time - 5.0f);
            }
        }
    }
}

bool processKey(Music& music) {
    if (cv::getWindowProperty("a2i", cv::WND_PROP_AUTOSIZE) == 1) {
        return 0;
    }

    int key = cv::waitKey(1);

    if (key == ' ') {
        if (PAUSE) PauseMusicStream(music);
        else ResumeMusicStream(music);
        PAUSE = !PAUSE;
    }

    if (key == 27 || key == 'q' || key == 'Q') {
        RUNNING = false;
        return 0;
    }
    if (key == 83 || key == 'd' || key == 'D') {
        float current_time = GetMusicTimePlayed(music);
        SeekMusicStream(music, current_time + 5.0f);
    }
    if (key == 81 || key == 'a' || key == 'A') {
        float current_time = GetMusicTimePlayed(music);
        SeekMusicStream(music, current_time - 5.0f);
    }

    return 1;
}

void processAudio() {
  w.img = cv::Mat(WINDOW.first, WINDOW.second, CV_8UC3, cv::Scalar(22, 16, 20));
  if (config.get<bool>("grid")) cv::add(w.grid, w.img, w.img);
  w.cur_img = cv::Mat::zeros(WINDOW.first, WINDOW.second, CV_8UC3);
  
  g.drawSpectrum(w.cur_img, config.get<int>("l"), config.get<int>("g"), config.get<int>("fill"), config.get<bool>("b"), config.get<cv::Scalar>("lc"), config.get<cv::Scalar>("ulc"), config.get<int>("gc"));

  if(config.get<int>("n") > 0)
  {
    for (int i = config.get<int>("n") - 1; i >= 0; --i) {
        cv::addWeighted(w.img, 1.0, w.prev[i], 0.3 / (i+1), 0.0, w.img);
    }
  }


  cv::add(w.cur_img, w.img, w.img);
  if (config.get<int>("grad") > -1) cv::applyColorMap(w.img, w.img, config.get<int>("grad"));

  cv::imshow("a2i", w.img);

  if(config.get<int>("n") > 0)
  {
    for (int i = config.get<int>("n") - 1; i > 0; --i) {
        w.prev[i] = w.prev[i - 1].clone();
    }

    w.cur_img.copyTo(w.prev[0]);
  }
}

int main(int argc, char** argv) {
    Parser parser(argc, argv, config);

    // std::cout << config.configJson << std::endl;

    if(config.end) {
      return 0;
    }

    if(!config.validate()) {
      std::cout << "Error: !config.validate().\n";
      //написать внутри валидейта проверки и прокидывания исключений
      return 0;
    }

    if(!config.execute()) {
      return 0;
    }


    auto file = config.get<std::string>("audiofile");
    if (file.empty()) {
        std::cout << "Error: No audio file specified.\n";
        parser.printUsage();
        return 0;
    }
    const char *file_path = file.c_str();

    ONLY_AUDIO = config.get<bool>("onlyaudio");
    DEBUG_MODE = config.get<bool>("debug");
    MIC_MODE = config.get<std::string>("command") == "mic" ? true : false;
    WINDOW = config.get<std::pair<int, int>>("s");
    FRAME_SIZE = config.get<unsigned int>("f");

    if (!DEBUG_MODE) {
        SetTraceLogLevel(LOG_WARNING);
    }

    InitAudioDevice();
    AudioStream stream;
    Music music;

    if (MIC_MODE && !ONLY_AUDIO) {
        stream = LoadAudioStream(44100, 16, 1);
        PlayAudioStream(stream);
    } else {
        music = LoadMusicStream(file_path);
        PlayMusicStream(music);
        SetMusicVolume(music, config.get<float>("v"));
        if (!ONLY_AUDIO) {
            AttachAudioStreamProcessor(music.stream, callback);
        }
    }

    if (!ONLY_AUDIO) {
        g.setAudioInfo(music.stream.sampleRate, config.get<std::pair<int, int>>("a"));
        g.setFreqRange({20, 20000}); // это тоже флаг сделать
        g.setFrameSize(FRAME_SIZE);
        g.setWindowFunc(config.get<int>("f"));
        cv::namedWindow("a2i", cv::WINDOW_NORMAL);
        cv::resizeWindow("a2i", WINDOW.second, WINDOW.first);
    } else {
        parser.printControls();
    }

    std::thread consoleInputThread(handleConsoleInput, std::ref(music), std::ref(RUNNING));

    w.img = cv::Mat(WINDOW.first, WINDOW.second, CV_8UC3, cv::Scalar(22, 16, 20));
    w.prev = std::vector<cv::Mat>(config.get<int>("n"), cv::Mat::zeros(WINDOW.first, WINDOW.second, CV_8UC3));

    w.cur_img = cv::Mat::zeros(WINDOW.first, WINDOW.second, CV_8UC3);
    w.grid = cv::Mat::zeros(WINDOW.first, WINDOW.second, CV_8UC3);
    if (config.get<bool>("grid")) g.drawGrid(w.grid, config.get<int>("g"), 1, {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000}, 10, config.get<cv::Scalar>("glc"), config.get<cv::Scalar>("gtc"));

    while (RUNNING) {
        if (MIC_MODE && !ONLY_AUDIO) {
            unsigned int frames = config.get<unsigned int>("FrameSize");
            Frame *buffer = (Frame *)malloc(frames * sizeof(Frame));
            if (IsAudioStreamProcessed(stream)) {
                UpdateAudioStream(stream, buffer, frames);
                callback(buffer, frames);
            }
            free(buffer);
        } else {
            UpdateMusicStream(music);

            if (!ONLY_AUDIO) {
              if(!processKey(music)) break;
            }
        }

        if (SHOW && !ONLY_AUDIO) {
            processAudio();
        }
    }

    consoleInputThread.join();

    if (MIC_MODE && !ONLY_AUDIO) {
        StopAudioStream(stream);
        UnloadAudioStream(stream);
    } else {
        UnloadMusicStream(music);
    }

    CloseAudioDevice();

    cv::destroyAllWindows();

    return 0;
}
