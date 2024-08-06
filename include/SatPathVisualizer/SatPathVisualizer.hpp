#pragma once

#include <raylib.h>
#include <raylib-cpp.hpp>
#include <H5Cpp.h>
#include <opencv2/opencv.hpp>

#define SPHERE_RES 64
#define TEXTURE_SIZE 2048

#define MAX_SATELLITES 10

#define SATELLITE_ALTITUDE 1.15

extern int ROWSTEP;
extern int COLSTEP;

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
    std::vector<hsize_t> dims;
    std::vector<Vector2> satPos;
    std::vector<Vector2> satSwath;
    //std::vector<float> satSwathTimes;
    raylib::RenderTexture2D satRenderTexture;
    Satellite(H5::H5File file, int satIndex, Color trailColor, Color swathColor);
    SatAttr getSatAttr(H5::H5File file, std::string datasetName);
    void updateSwaths(hsize_t dataIndexCol, hsize_t dataIndexRow);
    void drawSatTrail(hsize_t dataIndexCol, hsize_t dataIndexRow);
};

std::vector<Satellite> getSatellites(std::string fileName);

// GenMeshUVSphere.cpp
Mesh GenMeshUVSphere(float radius, int rings, int slices);

// Visualizer.cpp
cv::Mat textureToMat(Texture2D texture);

class Visualizer {
public:
    Visualizer();
    void render(int screenWidth, int screenHeight, int renderFPS, std::string outputFile, std::string sphereTextureFile, std::string satellitesFile, std::string satelliteTextureFile, float satelliteTextureSize, int satellitesCount, bool saveRender = false);
};
