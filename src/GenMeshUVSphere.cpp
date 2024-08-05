#include "SatPathVisualizer/SatPathVisualizer.hpp"
#include <raymath.h>
#include <vector>

Mesh GenMeshUVSphere(float radius, int rings, int slices) {
    Mesh mesh = {};
    if (rings < 3 || slices < 3) {
        TraceLog(LOG_WARNING, "Failed to generate UV sphere mesh");
        return mesh;
    }

    std::vector<Vector3> vertices;
    std::vector<Vector2> texcoords;
    std::vector<Vector3> normals;
    std::vector<ushort> indices;

    for (int i = rings - 1; i >= 0; i--) {
        for (int j = 0; j <= slices; j++) {
            texcoords.push_back({(float)j/slices, (float)i/(rings-1)});
        }
    }

    for (int i = 0; i < texcoords.size(); i++) {
        float longitude = 360*texcoords[i].x - 180;
        float latitude = 180*texcoords[i].y - 90;
        normals.push_back(convertLonLat3D(longitude, latitude));
    }

    for (int i = 0; i < normals.size(); i++) {
        vertices.push_back(Vector3Scale(normals[i], radius));
    }

    mesh.vertexCount = vertices.size();
    mesh.triangleCount = 0;

    for (int i = 0; i < slices; i++) {
        indices.push_back(i);
        indices.push_back(slices+i);
        indices.push_back(slices+i+1);
        mesh.triangleCount++;
    }
    for (int i = 1; i + 2 < rings; i++) {
        for (int j = 0; j < slices; j++) {
            indices.push_back(i*slices+j);
            indices.push_back((i+1)*slices+j);
            indices.push_back((i+1)*slices+j+1);
            indices.push_back((i+1)*slices+j+1);
            indices.push_back(i*slices+j+1);
            indices.push_back(i*slices+j);
            mesh.triangleCount += 2;
        }
    }
    for (int i = 0; i < slices; i++) {
        indices.push_back((rings-2)*slices+i);
        indices.push_back((rings-1)*slices+i);
        indices.push_back((rings-2)*slices+i+1);
        mesh.triangleCount++;
    }

    mesh.vertices = new float[vertices.size()*3];
    mesh.texcoords = new float[texcoords.size()*2];
    mesh.normals = new float[normals.size()*3];
    mesh.indices = new ushort[indices.size()];

    if (mesh.vertices == nullptr || mesh.texcoords == nullptr || mesh.normals == nullptr || mesh.indices == nullptr) {
        TraceLog(LOG_WARNING, "Failed to generate UV sphere mesh");
        return mesh;
    }

    memcpy(mesh.vertices, vertices.data(), vertices.size()*sizeof(Vector3));
    memcpy(mesh.texcoords, texcoords.data(), texcoords.size()*sizeof(Vector2));
    memcpy(mesh.normals, normals.data(), normals.size()*sizeof(Vector3));
    memcpy(mesh.indices, indices.data(), indices.size()*sizeof(ushort));

    UploadMesh(&mesh, false);

    return mesh;
}
