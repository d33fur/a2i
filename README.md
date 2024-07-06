# a2i: Audio to Image

```
_____/\\\\\\\\\_______/\\\\\\\\\______/\\\\\\\\\\\_        
 ___/\\\\\\\\\\\\\___/\\\///////\\\___\/////\\\///__       
  __/\\\/////////\\\_\///______\//\\\______\/\\\_____      
   _\/\\\_______\/\\\___________/\\\/_______\/\\\_____     
    _\/\\\\\\\\\\\\\\\________/\\\//_________\/\\\_____    
     _\/\\\/////////\\\_____/\\\//____________\/\\\_____   
      _\/\\\_______\/\\\___/\\\/_______________\/\\\_____  
       _\/\\\_______\/\\\__/\\\\\\\\\\\\\\\__/\\\\\\\\\\\_ 
        _\///________\///__\///////////////__\///////////__
```

![Demo of a2i](assets/demo.gif)

## Table of Contents

- [Changelog](CHANGELOG.md)
- [Overview](#overview)
  - [Utility](#utility)
  - [Library](#library)
- [Utility features](#utility-features)
- [Library features](#library-features)
- [Requirements](#requirements)
- [Installation](#installation)
  - [Linux/MacOS](#linuxmacos)
  - [Windows](#windows)
- [Usage](#usage)
  - [Command Line Options](#command-line-options)
  - [Controls](#controls)
  - [Examples](#examples)
- [Configuration](#configuration)
- [API Reference](#api-reference)
  - [CMake](#cmake)
  - [Initialization](#initialization)
  - [Generating Spectrograms](#generating-spectrograms)
  - [Customizing Parameters](#customizing-parameters)
- [FAQ](#faq)
- [Troubleshooting](#troubleshooting)
  - [Common Issues](#common-issues)
  - [Reporting Issues](#reporting-issues)
- [Future Plans](#future-plans)
- [Contributing](#contributing)
- [License](#license)

## Overview

`a2i` is a powerful utility and library for generating and visualizing audio spectrograms. The utility leverages the capabilities of `raylib` for audio processing and `OpenCV` for image manipulation and visualization, while the library itself focuses on providing robust spectrogram generation capabilities using `FFTW` for fast Fourier transforms.

### Utility

The `a2i` utility is designed for users who want an out-of-the-box solution for audio spectrogram visualization. It provides real-time spectrogram visualization from audio files or live microphone input. The utility offers a variety of customizable parameters, such as window functions, frame sizes, and colormaps, allowing users to tailor the visualization to their specific needs. `raylib` is utilized for efficient audio processing, enabling smooth real-time performance, while `OpenCV` is used for rendering and manipulating the spectrogram images.

### Library

The `a2i` library provides the core functionality for generating audio spectrograms, which can be integrated into other applications. It offers a flexible API for processing audio data and generating spectrograms, without depending on `raylib`. Developers can use this library to build custom audio analysis tools, integrate spectrogram visualization into existing projects, or perform advanced audio processing tasks. The library supports various window functions and allows for detailed customization of the spectrogram generation process.

By separating the utility and library, `a2i` caters to both end-users looking for a ready-to-use tool and developers seeking to integrate spectrogram capabilities into their own applications.

## Utility features

- **Real-Time Spectrogram Visualization**: Visualize spectrograms in real-time from audio files or live microphone input.
- **Customizable Parameters**: Adjust window functions, frame sizes, colormaps, and more.
- **Keyboard Controls**: Control playback with intuitive keyboard shortcuts (pause, rewind, fast forward, etc.).
- **Audio Processing with `raylib`**: Leverage `raylib` for efficient and smooth audio processing.
- **Image Manipulation with `OpenCV`**: Use `OpenCV` for robust image rendering and manipulation.
- **Command Line Interface**: A comprehensive CLI for specifying input files, setting visualization parameters, and controlling output.
- **Visual Feedback**: Provides immediate visual feedback for audio data analysis.

## Library features

- **Core Spectrogram Generation**: Generate detailed spectrograms from audio data.
- **Flexible API**: Integrate spectrogram generation into custom applications with a flexible API.
- **Customizable Processing**: Support for various window functions and detailed customization options.
- **Fast Fourier Transforms with `FFTW`**: Utilizes `FFTW` for fast and efficient Fourier transform computations.
- **No External Dependencies**: Focuses on spectrogram generation without relying on `raylib` for audio processing. You can use any other frameworks or libraries for audio handling.
- **Advanced Audio Processing**: Suitable for building custom audio analysis tools and integrating into existing projects.
- **Extensible and Modular**: Designed to be easily extended and integrated with other libraries and frameworks.

## Requirements

Ensure your system is up to date and has the necessary dependencies:
```sh
sudo apt update && sudo DEBIAN_FRONTEND=noninteractive apt install -y make cmake build-essential pkg-config git libfftw3-dev libopencv-dev pulseaudio libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
```

## Installation

### Linux/MacOS

1. **Clone the repository:**
```sh
git clone https://github.com/d33fur/a2i.git && cd a2i
```

2. **Build and install:**
```sh
make
```

3. **Run the executable:**
```sh
a2i <audio-file> [options]
```

### Windows

To be added.

## Usage

### Command Line Options

```
Usage:
  a2i <path> [options]
Options:
  -a <range>               Amplitude range (default: -90,50)
  -w <int>                 Window function (0-9, default: 9)
  -l <int>                 Line type (0-2, default: 0)
  -g <int>                 Graph mode (0-1, default: 1)
  -m <int>                 Normalize multiplier (default: 20)
  -mic                     Use microphone
  -f <int>                 Frame size (>=512, default: 65536)
  -n <int>                 Number of previous frames (>0)
  -s <height,width>     Window size (default: 400,2100)
  -grad <int>              Colormap (0-21)
  -fill <int>              Fill type (0-2, default: 2)
  -b                  Border line
  -lc <color>            Line color (default: 255,255,255)
  -uc <color>       Underline color (default: 127,127,127)
  -grid                    Grid
  -gc <int>         Gradient coefficient (0-255, default: 127)
  -glc <color> Grid line color
  -gtc <color> Grid text color
  -v <float>          Set volume level (0.0-1.0)
  -debug                   Enable debug mode
```

### Controls

- `Space`: Pause/Resume
- `Left Arrow`: Rewind 5 seconds
- `Right Arrow`: Fast Forward 5 seconds
- `Esc`: Exit

## Examples

### Visualize a real-time spectrogram from an audio file
To visualize a spectrogram with specific settings, use the following command:
```sh
a2i myaudiofile.wav -a=-90,100 -n=20 -size=400,1000 -grad=17 -grad_coef=255 -grid
```

### Real-time microphone input visualization
For real-time spectrogram visualization using your microphone:
```sh
a2i -mic -a=-90,100 -n=20 -size=400,1000 -grad=17 -grad_coef=255 -grid
```

### Changing the window function and amplitude range
```sh
a2i myaudiofile.wav -w=7 -a=-80,70 -size=500,1200 -grad=10 -grid
```

## Configuration

You can configure various parameters in `config.ini` to customize the behavior of `a2i`. Here's an example configuration:

```ini
[General]
AmplitudeRange=-90,50
WindowFunction=9
LineType=0
GraphMode=1
NormalizeMultiplier=20
FrameSize=65536
NumberOfPreviousFrames=10
WindowSize=400,2100
Colormap=17
FillType=2
BorderLine=1
LineColor=255,255,255
UnderlineColor=127,127,127
Grid=1
GradientCoefficient=127
GridLineColor=128,128,128
GridTextColor=200,200,200
```


## API Reference

### CMake

```CMakeLists.txt
find_package(a2i REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC a2i::a2i)
```

### Initialization

```cpp
#include <a2i/spectrogram.hpp>

// Create an object of the Spectrogram class from the a2i library
a2i::Spectrogram g;

// Set audio information, including the sample rate and amplitude range
g.setAudioInfo(sample_rate, {-90, 50});

// Set the frequency range for the spectrogram (by default from 20 Hz to 20,000 Hz)
g.setFreqRange({20, 20000});

// Set the frame size (number of samples in one frame)
g.setFrameSize(frame_size);

// Set the window function used for computing the spectrogram (possible options can be seen in the documentation)
g.setWindowFunc(window_function);
```

### Generating Spectrograms

```cpp
// Loop through each frame of audio data
for(size_t i = 0; i < frame_size; ++i) 
{
  // Add the average of the left and right audio channels to the input buffer
  g.in.push_back((frames[i].left + frames[i].right) / 2);
}

// Apply the window function to the audio data
g.addWindow();

// Perform the Fast Fourier Transform (FFT) on the windowed audio data
g.fft();

// Normalize the FFT output for visualization
g.normalize(multiplier);
```

### Customizing Parameters

```cpp
// Create an OpenCV matrix (image) with specified height, width, and type (8-bit, 3-channel)
cv::Mat img(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, cv::Scalar(22, 16, 20));

// Draw the spectrogram on the image with specified parameters (more details in the documentation)
g.drawSpectrum(img, line_type, graph_mode, fill_type, border_line, line_color, underline_color, gradient_coefficient);
```

## FAQ

### How do I install additional dependencies?

Follow the instructions in the [Requirements](#requirements) section to ensure all dependencies are installed.

### Can I use `a2i` on Windows or macOS?

Support for Windows is planned. For now, `a2i` is primarily tested on Linux and MacOS.

## Troubleshooting

### Common Issues
(добавить про пульсаудио)
- **Audio file not found**: Ensure the path to the audio file is correct.
- **Dependency issues**: Make sure all dependencies are installed as specified in the [Requirements](#requirements) section.

### Reporting Issues

If you encounter any issues, please open an issue on the [GitHub repository](https://github.com/d33fur/a2i/issues).

## Future Plans

- Support for Windows
- Enhanced visualization options
- Remove FFTW dependency(or change it to different library)
- Deb package for installing via apt

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
