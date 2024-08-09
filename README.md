# SatPathVisualizer

[SatPathVisualizer](https://github.com/FallingSky65/SatPathVisualizer) is a C++ library for creating satellite orbit path visualizations.

## Installing Dependencies

SatPathVisualizer depends on [cmake](https://cmake.org), [raylib](https://github.com/raysan5/raylib), [raylib-cpp](https://github.com/RobLoach/raylib-cpp), [hdf5](https://github.com/HDFGroup/hdf5), and [OpenCV](https://github.com/opencv/opencv). To use SatPathVisualizer, install raylib, hdf5, and OpenCV on your system.

### macOS (Homebrew)

1. Install [Homebrew](https://brew.sh/) (If you haven't already)
``` bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Install dependencies with Homebrew
``` bash
brew install cmake raylib hdf5 opencv
```

### Ubuntu / Debian

1. Install cmake
``` bash
sudo apt update && sudo apt upgrade
sudo apt install cmake
```

2. Install raylib by following the [raylib wiki](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux). Make sure to build using CMake (You may need to also install pkg-config first if you get errors configuring the build: `sudo apt install pkg-config`, and if your ubuntu/debian distribution uses Wayland, make sure to build raylib for wayland)

3. Install hdf5, opencv 
``` bash
sudo apt update && sudo apt upgrade
sudo apt install libhdf5-dev libopencv-dev
```

### Fedora

1. Install packages
``` bash
sudo dnf install cmake raylib-devel hdf5-devel opencv-devel
```

### Arch Linux

1. Install packages with pacman (or your preferred AUR helper)
``` bash
sudo pacman -Syu
sudo pacman -S cmake raylib hdf5 fmt vtk ffmpeg opencv
```

## Example

To use SatPathVisualizer, you can clone and edit the [example project](https://github.com/FallingSky65/SatPathVisualizerExample).
