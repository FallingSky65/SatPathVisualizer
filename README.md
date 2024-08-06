# SatPathVisualizer

[SatPathVisualizer](https://github.com/FallingSky65/SatPathVisualizer) is a C++ library for creating satellite orbit path visualizations.

## Getting Started

SatPathVisualizer depends on [raylib](https://github.com/raysan5/raylib), [raylib-cpp](https://github.com/RobLoach/raylib-cpp), [hdf5](https://github.com/HDFGroup/hdf5), and [OpenCV](https://github.com/opencv/opencv). To use SatPathVisualizer, install raylib, hdf5, and OpenCV on your system.

### macOS (Homebrew)

1. Install [Homebrew](https://brew.sh/)
``` bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Install dependencies with Homebrew
``` bash
brew install raylib hdf5 opencv
```

### Ubuntu

1. Install raylib by following the [raylib wiki](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux) (You may need to also install pkg-config first if you get errors configuring the build: `sudo apt install pkg-config`)

2. Install hdf5 
``` bash
sudo apt update
sudo apt install libhdf5-dev
```

3. Install opencv 
``` bash
sudo apt update
sudo apt install libopencv-dev
```

## Example

To use SatPathVisualizer, you can clone and edit the [example project](https://github.com/FallingSky65/SatPathVisualizerExample).
