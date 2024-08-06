cmake_minimum_required(VERSION 3.5)
project(SatPathVisualizer)

# Find raylib (needs to be installed by user)
find_package(raylib REQUIRED)

# Find raylib-cpp (does not need to be installed by user, will clone and build statically)
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v5.0.2
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

# Find HDF5 C++ (needs to be installed by user)
find_package(HDF5 COMPONENTS C CXX REQUIRED)

# Find OpenCV (needs to be installed by user)
find_package(OpenCV REQUIRED)

set(SOURCES
    src/Utils.cpp
    src/Satellite.cpp
    src/GenMeshUVSphere.cpp
    src/Visualizer.cpp
)

add_library(SatPathVisualizer STATIC ${SOURCES})
target_include_directories(SatPathVisualizer PUBLIC ${PROJECT_SOURCE_DIR}/include ${HDF5_INCLUDE_DIRS})
target_link_libraries(SatPathVisualizer PUBLIC raylib raylib_cpp ${HDF5_CXX_LIBRARIES} ${OpenCV_LIBS})
