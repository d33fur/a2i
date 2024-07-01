#include "raylib.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <a2i/spectrogram.hpp>

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
unsigned int FRAME_SIZE;
a2i::Spectrogram g;

int multiplier;
bool show = false;


typedef struct 
{
  float left;
  float right;
} Frame;

void callback(
  void *bufferData, 
  unsigned int frames) 
{
  if(frames < 512) return;

  Frame *fs = static_cast<Frame*>(bufferData);

  if(g.in.size() == FRAME_SIZE)
  {
    g.in.erase(g.in.begin(), g.in.begin() + frames);
  }

  for(size_t i = 0 ; i < frames; ++i) 
  {
    g.in.push_back((fs[i].left + fs[i].right) / 2);
  }

  if(g.in.size() == FRAME_SIZE)
  {
    g.addWindow();
    g.fft();
    g.normalize(multiplier);
    show = true;
  }
  else
  {
    show = false;
  }
}

cv::Scalar parseColor(const std::string& colorStr) 
{
  std::vector<int> values;
  std::stringstream ss(colorStr);
  std::string item;

  while(std::getline(ss, item, ',')) 
  {
    try 
    {
        values.push_back(std::stoi(item));
    } catch (const std::invalid_argument& e) 
    {
      throw std::invalid_argument("Invalid color component: " + item);
    }
  }

  if(values.size() != 3) 
  {
    throw std::invalid_argument("Invalid color string: " + colorStr);
  }

  return cv::Scalar(values[0], values[1], values[2]);
}

std::pair<int, int> parseRange(const std::string& str) 
{
  std::istringstream iss(str);
  int first, second;
  char comma;

  if(!(iss >> first >> comma >> second) || (comma != ',')) 
  {
    throw std::invalid_argument("Invalid string format");
  }

  return std::make_pair(first, second);
}

bool isPowerOfTwo(int n) {
  if(n < 512) return false;
  while(n % 2 == 0) 
  {
    n /= 2;
  }
  return n == 1;
}

void printUsage()
{
  std::cout << "Usage: \n"
            << "  a2i <path> [options]\n"
            << "Options:\n"
            << "  -a <range>              Amplitude range (default: -90,50)\n"
            << "  -w <int>                Window function (0-9, default: 9)\n"
            << "  -l <int>                Line type (0-2, default: 0)\n"
            << "  -g <int>                Graph mode (0-1, default: 1)\n"
            << "  -m <int>                Normalize multiplier (default: 20)\n"
            << "  -mic                    Use microphone\n"
            << "  -f <int>                Frame size (>=512, default: 65536)\n"
            << "  -n <int>                Number of previous frames (>0)\n"
            << "  -size <height,width>    Window size (default: 400,2100)\n"
            << "  -grad <int>             Colormap (0-21)\n"
            << "  -fill <int>             Fill type (0-2, default: 2)\n"
            << "  -border                 Border line\n"
            << "  -line <color>           Line color (default: 255,255,255)\n"
            << "  -underline <color>      Underline color (default: 127,127,127)\n"
            << "  -grid                   Grid\n"
            << "  -grad_coef <int>        Gradient coefficient (0-255, default: 127)\n"
            << "  -grid_line_color <color> Grid line color\n"
            << "  -grid_text_color <color> Grid text color\n"
            << "  -h, -help               Show this help message\n"
            << "Controls:\n"
            << "  Space                   Pause/Resume\n"
            << "  Left Arrow              Rewind 5 seconds\n"
            << "  Right Arrow             Fast Forward 5 seconds\n"
            << "  Esc                     Exit\n";
}

int main(int argc, char** argv) 
{
  cv::CommandLineParser parser(argc, argv,
  "{@input           |               | path to audiofile             }"
  "{ a               |    -90,50     | amplitude range               }"
  "{ w               |       9       | window function(0-9)          }"
  "{ l               |       0       | line type(0-2)                }"
  "{ g               |       1       | graph mode(0-1)               }"
  "{ m               |       20      | mormalize multiplier          }"
  "{ mic             |               | use microphone                }"
  "{ f               |     65536     | frame size(>=512)             }"
  "{ n               |               | number of prev frames(>0)     }"
  "{ size            |   400,2100    | window size(height,width)     }"
  "{ grad            |               | colormap(0-21)                }"
  "{ fill            |       2       | fill type(0-2)                }"
  "{ border          |               | border line                   }"
  "{ line            |  255,255,255  | line color(255,255,255)       }"
  "{ underline       |  127,127,127  | underline color(127,127,127)  }"
  "{ grid            |               | grid                          }"
  "{ grad_coef       |      127      | gradient coefficient(0-255)   }"
  "{ grid_line_color |   79,73,80    | grid line color               }"
  "{ grid_text_color |  51,186,243   | grid text color               }"
  "{ h help          |               | show help message             }");

  if(argc == 1 || parser.has("h") || parser.has("help"))
  {
    printUsage();
    return 0;
  }

  auto file = parser.get<std::string>("@input");
  if(file.empty())
  {
    std::cout << "Error: No audio file specified.\n";
    printUsage();
    return 0;
  }
  const char *file_path = file.c_str();
  auto amp = parseRange(parser.get<std::string>("a"));

  auto window_function = parser.get<int>("w");
  if(window_function < 0 || window_function > 9)
  {
    std::cout << "Invalid -w option" << '\n';
    std::cout << "Should be in range (0-9)" << '\n';
    return 0;
  }

  auto line_type = parser.get<int>("l");
  if(line_type  < 0 || line_type  > 2)
  {
    std::cout << "Invalid -l option" << '\n';
    std::cout << "Should be in range (0-2)" << '\n';
    return 0;
  }

  auto graph_mode = parser.get<int>("g");
  if(graph_mode  < 0 || graph_mode  > 1)
  {
    std::cout << "Invalid -g option" << '\n';
    std::cout << "Should be in range (0-1)" << '\n';
    return 0;
  }

  multiplier = parser.get<int>("m");
  auto use_mic = parser.has("mic");

  FRAME_SIZE = parser.get<unsigned int>("f");
  if(FRAME_SIZE < 512 || (FRAME_SIZE & (FRAME_SIZE - 1)) != 0)
  {
    std::cout << "Invalid -f option" << '\n';
    std::cout << "Should be >= 512 and power of 2 (512, 1024, 2048 etc)" << '\n';
    return 0;
  }

  auto bool_num_frames = parser.has("n");
  auto num_frames = parser.get<int>("n");
  if(bool_num_frames && num_frames <= 0)
  {
    std::cout << "Invalid -n option" << '\n';
    std::cout << "Should be > 0" << '\n';
    return 0;
  }

  auto window_size = parseRange(parser.get<std::string>("size"));
  WINDOW_HEIGHT = window_size.first;
  WINDOW_WIDTH = window_size.second;
  if(WINDOW_HEIGHT <= 0 || WINDOW_WIDTH <= 0)
  {
    std::cout << "Invalid -size option" << '\n';
    std::cout << "Both variables should be > 0" << '\n';
    return 0;
  }

  auto grad = parser.has("grad");
  auto colormap = parser.get<int>("grad");
  if(grad && (colormap < 0 || colormap > 21))
  {
    std::cout << "Invalid -grad option" << '\n';
    std::cout << "Should be in range (0-21)" << '\n';
    return 0;
  }

  auto fill_type = parser.get<int>("fill");
  if(fill_type < 0 || fill_type > 2)
  {
    std::cout << "Invalid -fill option" << '\n';
    std::cout << "Should be in range (0-2)" << '\n';
    return 0;
  }

  auto border_line = parser.has("border");
  auto line_color = parseColor(parser.get<std::string>("line"));
  auto underline_color = parseColor(parser.get<std::string>("underline"));
  auto grid_enabled = parser.has("grid");

  auto grad_coefficient = parser.get<int>("grad_coef");
  if(grad_coefficient < 0 || grad_coefficient > 255)
  {
    std::cout << "Invalid -grad_coef option" << '\n';
    std::cout << "Should be in range (0-255)" << '\n';
    return 0;
  }
  
  auto grid_line_color = parseColor(parser.get<std::string>("grid_line_color"));
  auto grid_text_color = parseColor(parser.get<std::string>("grid_text_color"));

  bool stop = true;

  InitAudioDevice();
  AudioStream stream;
  Music music;

  if(use_mic) 
  {
    stream = LoadAudioStream(44100, 16, 1);
    PlayAudioStream(stream);
  } 
  else 
  {
    music = LoadMusicStream(file_path);
    PlayMusicStream(music);
    SetMusicVolume(music, 0.8f);
    AttachAudioStreamProcessor(music.stream, callback);
  }

  g.setAudioInfo(music.stream.sampleRate, amp);
  g.setFreqRange({20, 20000});
  g.setFrameSize(FRAME_SIZE);
  g.setWindowFunc(window_function);

  cv::namedWindow("a2i", cv::WINDOW_NORMAL);
  cv::resizeWindow("a2i", WINDOW_WIDTH, WINDOW_HEIGHT);

  cv::Mat img(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, cv::Scalar(22, 16, 20));
  if(!bool_num_frames) num_frames = 1;
  std::vector<cv::Mat> prev_frames(num_frames, cv::Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3));

  cv::Mat cur_img = cv::Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
  cv::Mat grid = cv::Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
  if(grid_enabled) g.drawGrid(grid, graph_mode, 1, {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000}, 10, grid_line_color, grid_text_color);

  while(true)
  {
    if(use_mic) 
    {
      unsigned int frames = FRAME_SIZE;
      Frame *buffer = (Frame *)malloc(frames * sizeof(Frame));
      if(IsAudioStreamProcessed(stream))
      {
        UpdateAudioStream(stream, buffer, frames);
        callback(buffer, frames);
      }
      free(buffer);
    } 
    else
    {
      UpdateMusicStream(music);

      int key;

      key = cv::waitKey(1);
      
      if(key == ' ') 
      {
        if(stop) PauseMusicStream(music);
        else ResumeMusicStream(music);
        stop = !stop;
      }

      if(key == 27)  // Esc 
      {
        break;
      }
      if(key == 83)  // Right arrow 
      {
        float current_time = GetMusicTimePlayed(music);
        SeekMusicStream(music, current_time + 5.0f);
      }
      if(key == 81)  // Left arrow 
      {
        float current_time = GetMusicTimePlayed(music);
        SeekMusicStream(music, current_time - 5.0f);
      }
    }

    if(show)
    {
      img = cv::Mat(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, cv::Scalar(22, 16, 20));
      if(grid_enabled) cv::add(grid, img, img);
      cur_img = cv::Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);

      g.drawSpectrum(cur_img, line_type, graph_mode, fill_type, border_line, line_color, underline_color, grad_coefficient);

      if(bool_num_frames)
      {
        for(int i = num_frames - 1; i >= 0; --i)
        {
          cv::addWeighted(img, 1.0, prev_frames[i], 0.3 / (i+1), 0.0, img);
        }
      }
      
      cv::add(cur_img, img, img);
      if(grad) cv::applyColorMap(img, img, colormap);
      
      cv::imshow("a2i", img);

      if(bool_num_frames)
      {
        for(int i = num_frames - 1; i > 0; --i)
        {
          prev_frames[i] = prev_frames[i - 1].clone();
        }

        cur_img.copyTo(prev_frames[0]);
      }
    }
  }

  if(use_mic) 
  {
    StopAudioStream(stream);
    UnloadAudioStream(stream);
  } 
  else 
  {
    UnloadMusicStream(music);
  }
  CloseAudioDevice();

  cv::destroyAllWindows();
  return 0;
}