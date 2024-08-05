#include "SatPathVisualizer/SatPathVisualizer.hpp"
#include <cstdlib>
#include <vector>

Satellite::Satellite(H5::H5File file, int satIndex, Color _trailColor, Color _swathColor) {
    satRenderTexture = raylib::RenderTexture2D(TEXTURE_SIZE*2, TEXTURE_SIZE);
    trailColor = _trailColor;
    swathColor = _swathColor;
    SatAttr lon = getSatAttr(file, TextFormat("/lon%i", satIndex));
    SatAttr lat = getSatAttr(file, TextFormat("/lat%i", satIndex));
    SatAttr lonSwath = getSatAttr(file, TextFormat("/lon%i_swath", satIndex));
    SatAttr latSwath = getSatAttr(file, TextFormat("/lat%i_swath", satIndex));
    //SatAttr lstSwath = getSatAttr(file, TextFormat("/lst%i_swath", satIndex));
    if (
        lon.rank != 1 ||
        lat.rank != 1 ||
        lonSwath.rank != 2 ||
        latSwath.rank != 2 ||
        //lstSwath.rank != 2 ||
        lon.dims[0] != lat.dims[0] ||
        lon.dims[0] != lonSwath.dims[0] ||
        lon.dims[0] != latSwath.dims[0] ||
        //lon.dims[0] != lstSwath.dims[0] ||
        lonSwath.dims[1] != latSwath.dims[1]// ||
        //lonSwath.dims[1] != lstSwath.dims[1]
    ) {
        TraceLog(LOG_WARNING, TextFormat("Sat %i does not have matching dimensions", satIndex));
    } else {
        dims = std::vector<hsize_t>(lonSwath.dims);
        satPos = std::vector<Vector2>(dims[0]);
        for (hsize_t i = 0; i < dims[0]; i++) satPos[i] = {lon.data[i], lat.data[i]};
        satSwath = std::vector<Vector2>(dims[0]*dims[1]);
        for (hsize_t i = 0; i < dims[0]*dims[1]; i++) satSwath[i] = {lonSwath.data[i], latSwath.data[i]};
        //satSwathTimes = std::vector<float>(lstSwath.data);
    }
}

SatAttr Satellite::getSatAttr(H5::H5File file, std::string datasetName) {
    SatAttr satAttr;
    try {
        H5::DataSet dataset = file.openDataSet(datasetName);
        H5::DataType datatype = dataset.getDataType();
        if (datatype != H5::PredType::NATIVE_FLOAT) {
            TraceLog(LOG_WARNING, TextFormat("Dataset '%s' type is not float", datasetName.c_str()));
            return satAttr;
        }
        H5::DataSpace dataspace = dataset.getSpace();
        satAttr.rank = dataspace.getSimpleExtentNdims();
        satAttr.dims = std::vector<hsize_t>(satAttr.rank);
        dataspace.getSimpleExtentDims(satAttr.dims.data(), NULL);
        hsize_t totalData = 1;
        for (uint i = 0; i < satAttr.rank; i++) totalData *= satAttr.dims[i];
        satAttr.data = std::vector<float>(totalData);
        dataset.read(satAttr.data.data(), datatype);
    } catch (H5::Exception &error) {
        error.printErrorStack();
        TraceLog(LOG_WARNING, TextFormat("Dataset '%s' not loaded properly", datasetName.c_str()));
    }
    return satAttr;
}

void Satellite::updateSwaths(hsize_t dataIndexCol, hsize_t dataIndexRow) {
    satRenderTexture.BeginMode();
    {
        for (hsize_t i = dataIndexRow; i <= dataIndexRow + ROWSTEP; i++) {
            for (hsize_t j = dataIndexCol; j <= dataIndexCol + COLSTEP && i*dims[1]+j < dims[0]*dims[1]; j++) {
                if (std::abs(satSwath[i*dims[1]+j].y) <= 35.0f) {
                    float x = (180.0f + satSwath[i*dims[1]+j].x)*TEXTURE_SIZE/180.0f;
                    float y = (90.0f - satSwath[i*dims[1]+j].y)*TEXTURE_SIZE/180.0f;
                    DrawCircle(x, y, 5.0f, swathColor);
                }
            }
        }
    }
    satRenderTexture.EndMode();
}

void Satellite::drawSatTrail(hsize_t dataIndexCol, hsize_t dataIndexRow) {
    for (hsize_t i = 0; i + ROWSTEP < dims[0]; i += ROWSTEP) {
        Vector3 start, end;
       start = convertLonLat3D(satPos[i], SATELLITE_ALTITUDE);
        if (i + ROWSTEP <= dataIndexRow) {
            end = convertLonLat3D(satPos[i + ROWSTEP], SATELLITE_ALTITUDE);
            DrawCylinderEx(start, end, 0.005f, 0.005f, 4, trailColor);
        } else {
            hsize_t j = std::min(dims[0] - 1, i + ROWSTEP*dataIndexCol/dims[1]);
            position = convertLonLat3D(satPos[j], SATELLITE_ALTITUDE);
            DrawCylinderEx(start, position, 0.005f, 0.005f, 4, trailColor);
            if (std::abs(satSwath[j*dims[1]+dataIndexCol].y) <= 35.0f) DrawCylinderEx(position, convertLonLat3D(satSwath[j*dims[1]+dataIndexCol]), 0.005f, 0.005f, 4, RED);
            break;
        }
    }
}

std::vector<Satellite> getSatellites(std::string fileName) {
    std::vector<Satellite> satellites;
    try {
        H5::H5File file(fileName, H5F_ACC_RDONLY);
        satellites.push_back(Satellite(file, 1, WHITE, RAYWHITE));
        satellites.push_back(Satellite(file, 2, BLUE, SKYBLUE));
    } catch (H5::Exception &error) {
        error.printErrorStack();
        TraceLog(LOG_WARNING, TextFormat("File '%s' did not load properly", fileName.c_str()));
    }

    return satellites;
}
