#pragma once

#include <opencv2/videoio.hpp>
#include <raylib.h>
#include <raylib-cpp.hpp>
#include <H5Cpp.h>
#include <opencv2/opencv.hpp>

#define SPHERE_RES 64
#define TEXTURE_SIZE 2048

#define MAX_SATELLITES 10

#define SATELLITE_ALTITUDE 1.15

#define TRAIL_STEP 40

// Utils.cpp
Vector3 convertLonLat3D(float longitude, float latitude);
Vector3 convertLonLat3D(float longitude, float latitude, float radius);
Vector3 convertLonLat3D(Vector2 v);
Vector3 convertLonLat3D(Vector2 v, float radius);
float mixDegree(float x, float y, float a);

// Satellite.hpp
struct SatAttr {
    uint rank;
    std::vector<hsize_t> dims;
    std::vector<float> data;
};

class Satellite {
public:
    Vector3 position;
    Color trailColor, swathColor;
    int prevSwathIndex;
    std::vector<hsize_t> dims;
    std::vector<Vector2> satPos;
    std::vector<Vector2> satSwath;
    //std::vector<float> satSwathTimes;
    raylib::RenderTexture2D satRenderTexture;
    Satellite(H5::H5File file, int satIndex, Color trailColor, Color swathColor);
    SatAttr getSatAttr(H5::H5File file, std::string datasetName);
    void updateSwaths(int swathIndex);
    void drawSatTrail(int positionIndex);
};

std::vector<Satellite> getSatellites(std::string fileName);

// GenMeshUVSphere.cpp
Mesh GenMeshUVSphere(float radius, int rings, int slices);

// Visualizer.cpp
cv::Mat textureToMat(Texture2D texture);

class Visualizer {
public:
    const int codec = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
    int renderFPS;
    std::string outputFile;
    cv::VideoWriter videoWriter;
    raylib::Window window;
    raylib::Camera3D camera;
    raylib::RenderTexture2D canvas;
    raylib::Model sphereModel;
    raylib::Texture2D sphereTexture;
    raylib::Shader* shader;
    std::vector<Satellite> satellites;

    Visualizer();
    void beginRender(int screenWidth, int screenHeight, int _renderFPS, std::string sphereTextureFile, std::string satellitesFile, int satellitesCount, std::string _outputFile);
    void endRender();
    void updateSatellites(int swathIndex);
    void updateCamera(int followSatellite, int positionIndex);
    void drawToCanvas(int positionIndex);
    void drawCanvasToScreen();
    void saveFrame();
};
