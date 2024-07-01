# a2i: Audio to Image Spectrogram Utility

## Overview

`a2i` is a powerful utility and library for generating and visualizing audio spectrograms. It leverages the capabilities of `raylib` for audio processing and `OpenCV` for image manipulation and visualization, along with the `a2i` library for spectrogram generation.

## Features

- Convert audio files to spectrogram images.
- Real-time spectrogram visualization from microphone input.
- Customizable spectrogram parameters (window function, amplitude range, etc.).
- Various visualization options (colormap, grid, line types, etc.).
- Control playback with keyboard inputs.

## Requirements

```
sudo apt update && sudo DEBIAN_FRONTEND=noninteractive apt install -y make build-essential pkg-config git libfftw3-dev libopencv-dev python3-pip libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev && pip3 install cmake
```

## Installation

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

## Usage

### Command Line Options

```
Usage:
  a2i <path> [options]
Options:
  -a <range>              Amplitude range (default: -90,50)
  -w <int>                Window function (0-9, default: 9)
  -l <int>                Line type (0-2, default: 0)
  -g <int>                Graph mode (0-1, default: 1)
  -m <int>                Normalize multiplier (default: 20)
  -mic                    Use microphone
  -f <int>                Frame size (>=512, default: 65536)
  -n <int>                Number of previous frames (>0)
  -size <height,width>    Window size (default: 400,2100)
  -grad <int>             Colormap (0-21)
  -fill <int>             Fill type (0-2, default: 2)
  -border                 Border line
  -line <color>           Line color (default: 255,255,255)
  -underline <color>      Underline color (default: 127,127,127)
  -grid                   Grid
  -grad_coef <int>        Gradient coefficient (0-255, default: 127)
  -grid_line_color <color> Grid line color
  -grid_text_color <color> Grid text color
  -h, -help               Show this help message
```

### Controls

- `Space`: Pause/Resume
- `Left Arrow`: Rewind 5 seconds
- `Right Arrow`: Fast Forward 5 seconds
- `Esc`: Exit

## Examples

1. **Visualize a real-time spectrogram from an audio file:**
```sh
a2i myaudiofile.wav -a=-90,100 -n=20 -size=400,1000 -grad=17 -grad_coef=255 -grid
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes. For major changes, please open an issue first to discuss what you would like to change.

## Acknowledgments

- [raylib](https://www.raylib.com/)
- [OpenCV](https://opencv.org/)