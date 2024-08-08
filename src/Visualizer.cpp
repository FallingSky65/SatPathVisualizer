#include "SatPathVisualizer/SatPathVisualizer.hpp"

#include <H5public.h>
#include <opencv2/videoio.hpp>
#include <raylib.h>
#include <raylib-cpp.hpp>
#include <opencv2/opencv.hpp>
#include <H5Cpp.h>
#include <raymath.h>
#include <vector>

cv::Mat textureToMat(Texture2D texture) {
    Image image = LoadImageFromTexture(texture);
    Color* pixels = LoadImageColors(image);

    cv::Mat mat(image.height, image.width, CV_8UC4);
    
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            Color pixel = pixels[y * image.width + x];
            mat.at<cv::Vec4b>(y, x) = cv::Vec4b(pixel.b, pixel.g, pixel.r, pixel.a);
        }
    }

    UnloadImageColors(pixels);
    UnloadImage(image);

    // Convert BGRA to BGR (optional, if you don't need the alpha channel)
    cv::Mat mat_bgr;
    cv::cvtColor(mat, mat_bgr, cv::COLOR_BGRA2BGR);

    return mat_bgr;
}

Visualizer::Visualizer() {
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
}

void Visualizer::beginRender(int screenWidth, int screenHeight, int _renderFPS, std::string sphereTextureFile, std::string satellitesFile, int satellitesCount, std::string _outputFile) {
    
    renderFPS = _renderFPS;
    outputFile = _outputFile;

    window.Init(screenWidth, screenHeight, TextFormat("SatPathVisualizer %ix%i %s", screenWidth, screenHeight, outputFile.c_str()));

    satellites = getSatellites(satellitesFile);

    camera.SetPosition({0.0f, 0.0f, 16.0f});
    camera.SetTarget({0.0f, 0.0f, 0.0f});
    camera.SetUp({0.0f, 1.0f, 0.0f});
    camera.SetFovy(2.5f);
    camera.SetProjection(CAMERA_ORTHOGRAPHIC);

    //canvas.Load(screenWidth, screenHeight);
    canvas = raylib::RenderTexture2D(screenWidth, screenHeight);

    sphereModel.Load(GenMeshUVSphere(1.0f, SPHERE_RES, SPHERE_RES*2));
    //sphereModel = raylib::Model(GenMeshUVSphere(1.0f, SPHERE_RES, SPHERE_RES*2));
    sphereTexture.Load(sphereTextureFile);
    //sphereTexture = raylib::Texture2D(sphereTextureFile);
    sphereModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = sphereTexture;

    for (int i = 0; i < satellites.size() && i < MAX_SATELLITES; i++)
        sphereModel.materials[0].maps[MATERIAL_MAP_METALNESS+i].texture = satellites[i].satRenderTexture.texture;
    
    //shader->Load("SatPathVisualizer/src/shaders/vert.glsl", "SatPathVisualizer/src/shaders/frag.glsl");
    shader = new raylib::Shader("SatPathVisualizer/src/shaders/vert.glsl", "SatPathVisualizer/src/shaders/frag.glsl");
    shader->locs[SHADER_LOC_VECTOR_VIEW] = shader->GetLocation("viewPos");
    for (int i = 0; i < MAX_SATELLITES; i++)
        shader->locs[SHADER_LOC_MAP_METALNESS+i] = shader->GetLocation(TextFormat("texture%i", i+1));

    sphereModel.materials[0].shader = *shader;
}

void Visualizer::endRender() {
    videoWriter.release();
}

void Visualizer::updateSatellites(int swathIndex) {
    for (int i = 0; i < satellites.size(); i++)
        satellites[i].updateSwaths(swathIndex);
}

void Visualizer::updateCamera(int followSatellite, int positionIndex) {
    if (followSatellite < 0 || followSatellite >= satellites.size()) {
        TraceLog(LOG_WARNING, "followSatellite out of range, no such satellite");
        return;
    }
    camera.SetPosition(Vector3RotateByAxisAngle({0.0, 0.0, 16.0}, camera.GetUp(), satellites[followSatellite].satPos[positionIndex].x*DEG2RAD));
    camera.SetTarget({0.0, 0.0, 0.0});
    shader->SetValue(shader->locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);
}

void Visualizer::drawToCanvas(int positionIndex) {
    canvas.BeginMode();
    {
        ClearBackground(BLACK);
        camera.BeginMode();
        {
            sphereModel.Draw({0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
            for (int i = 0; i < satellites.size(); i++)
                satellites[i].drawSatTrail(positionIndex);
        }
        camera.EndMode();
    }
    canvas.EndMode();
}

void Visualizer::drawCanvasToScreen() {
    window.BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        canvas.GetTexture().Draw();
        DrawFPS(10, 10);
    }
    window.EndDrawing();
}

void Visualizer::saveFrame() {
    cv::Mat frame = textureToMat(canvas.texture);

    if (!videoWriter.isOpened()) {
        cv::Size frameSize = frame.size();
        videoWriter.open(outputFile, codec, renderFPS, frameSize, true);
        if (!videoWriter.isOpened())
            TraceLog(LOG_WARNING, "Failed to open videoWriter");
    }

    videoWriter.write(frame);
}
