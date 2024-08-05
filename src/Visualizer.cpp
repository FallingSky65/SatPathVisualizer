#include "SatPathVisualizer/SatPathVisualizer.hpp"

#include <H5public.h>
#include <opencv2/videoio.hpp>
#include <raylib.h>
#include <raylib-cpp.hpp>
#include <opencv2/opencv.hpp>
#include <H5Cpp.h>
#include <raymath.h>
#include <vector>

int ROWSTEP = 40;
int COLSTEP = 24;
const int cutoff = 5000;

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

void Visualizer::render(int screenWidth, int screenHeight, int renderFPS, std::string outputFile, std::string sphereTextureFile, std::string satellitesFile, std::string satelliteTextureFile, int satellitesCount, bool saveRender) {

    hsize_t dataIndexRow = 0;
    hsize_t dataIndexCol = 0;

    int codec = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    cv::VideoWriter videoWriter;

    raylib::Window window(screenWidth, screenHeight, TextFormat("SatPathVisualizer %ix%i %s", screenWidth, screenHeight, outputFile.c_str()));

    std::vector<Satellite> satellites = getSatellites(satellitesFile);

    int frameCount = 0;
    while (dataIndexRow < satellites[0].dims[0]) {
        frameCount++;
        dataIndexCol += COLSTEP;
        if (dataIndexCol >= satellites[0].dims[1]) {
            dataIndexCol = 0;
            dataIndexRow += ROWSTEP;
            if (dataIndexRow >= cutoff) {
                ROWSTEP = 50;
                COLSTEP = 40;
            }
        }
    }
    const int totalFrameCount = frameCount;
    dataIndexRow = 0; dataIndexCol = 0;
    frameCount = 0;
    COLSTEP = 24;

    printf("duration: %i seconds\n", totalFrameCount/60);

    raylib::Camera3D camera(
        {0.0f, 0.0f, 4.0f},     // Position
        {0.0f, 0.0f, 0.0f},     // Target
        {0.0f, 1.0f, 0.0f},     // Up
        35.0f,                  // FOV Y
        CAMERA_PERSPECTIVE      // Camera type
    );
    raylib::RenderTexture2D canvas(screenWidth, screenHeight);

    raylib::Model sphereModel(GenMeshUVSphere(1.0f, SPHERE_RES, SPHERE_RES*2));
    raylib::Texture2D sphereTexture(sphereTextureFile);
    sphereModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = sphereTexture;
    for (int i = 0; i < satellites.size() && i < MAX_SATELLITES; i++)
        sphereModel.materials[0].maps[MATERIAL_MAP_METALNESS+i].texture = satellites[i].satRenderTexture.texture;

    raylib::Shader shader("SatPathVisualizer/src/shaders/vert.glsl", "SatPathVisualizer/src/shaders/frag.glsl");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = shader.GetLocation("viewPos");
    for (int i = 0; i < MAX_SATELLITES; i++)
        shader.locs[SHADER_LOC_MAP_METALNESS+i] = shader.GetLocation(TextFormat("texture%i", i+1));

    sphereModel.materials[0].shader = shader;

    Image satelliteImage = LoadImage(satelliteTextureFile.c_str());
    ImageFlipVertical(&satelliteImage);
    raylib::Texture2D satelliteTexture(satelliteImage);
    UnloadImage(satelliteImage);

    if (!saveRender) SetTargetFPS(renderFPS);

    // Steps of each loop iteration
    // 1. Update satellites
    // 2. Update camera
    // 3. Draw to canvas 
    // 4. Draw canvas to screen
    // 5. Save canvas as a frame of the video
    while (!window.ShouldClose() && dataIndexRow < satellites[0].dims[0]) {
        frameCount++;

        // 1. Update satellites
        for (int i = 0; i < satellites.size(); i++)
            satellites[i].updateSwaths(dataIndexCol, dataIndexRow);

        // 2. Update camera
        float deg1 = mixDegree(satellites[0].satPos[dataIndexRow].x, satellites[0].satPos[std::min(dataIndexRow+ROWSTEP, satellites[0].dims[0]-1)].x, (float)dataIndexCol/satellites[0].dims[1]);
        float deg2 = mixDegree(satellites[1].satPos[dataIndexRow].x, satellites[1].satPos[std::min(dataIndexRow+ROWSTEP, satellites[0].dims[0]-1)].x, (float)dataIndexCol/satellites[0].dims[1]);
        camera.SetPosition(Vector3RotateByAxisAngle({0.0, 0.0, 4.0}, camera.GetUp(), deg1*DEG2RAD));
        camera.SetTarget({0.0, 0.0, 0.0});
        shader.SetValue(shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);

        // 3. Draw to canvas
        canvas.BeginMode();
        {
            ClearBackground(BLACK);
            camera.BeginMode();
            {
                sphereModel.Draw({0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
                for (int i = 0; i < satellites.size(); i++)
                    satellites[i].drawSatTrail(dataIndexCol, dataIndexRow);
                for (int i = 0; i < satellites.size(); i++)
                    satelliteTexture.DrawBillboard(camera, Vector3Lerp(satellites[i].position, camera.position, 0.05), 0.07, WHITE);
            }
            camera.EndMode();
        }
        canvas.EndMode();

        dataIndexCol += COLSTEP;
        if (dataIndexCol >= satellites[0].dims[1]) {
            dataIndexCol = 0;
            dataIndexRow += ROWSTEP;
            if (dataIndexRow >= cutoff) {
                ROWSTEP = 50;
                COLSTEP = 40;
            }
        }

        window.SetTitle(TextFormat("SatPathVisualizer %ix%i %s %.2f minutes left", screenWidth, screenHeight, outputFile.c_str(), (float)(GetTime()/frameCount*totalFrameCount-GetTime())/60.0f));

        // 4. Draw canvas to screen
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            canvas.GetTexture().Draw();
            DrawFPS(10, 10);
        }
        EndDrawing();

        // 5. Save canvas as a frame of the video
        if (saveRender) {
            cv::Mat frame = textureToMat(canvas.texture);

            if (!videoWriter.isOpened()) {
                cv::Size frameSize = frame.size();
                videoWriter.open(outputFile, codec, renderFPS, frameSize, true);
                if (!videoWriter.isOpened())
                    TraceLog(LOG_WARNING, "Failed to open videoWriter");
            }

            videoWriter.write(frame);
        }
    }

    videoWriter.release();
}
