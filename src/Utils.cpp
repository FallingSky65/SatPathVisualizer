#include <SatPathVisualizer/SatPathVisualizer.hpp>
#include <raymath.h>
#include <cmath>

Vector3 convertLonLat3D(float longitude, float latitude) {
    longitude *= DEG2RAD; latitude *= DEG2RAD;
    return {sinf(longitude)*cosf(latitude), sinf(latitude), cosf(longitude)*cosf(latitude)};
}

Vector3 convertLonLat3D(float longitude, float latitude, float radius) { return Vector3Scale(convertLonLat3D(longitude, latitude), radius); }

Vector3 convertLonLat3D(Vector2 v) { return convertLonLat3D(v.x, v.y); }

Vector3 convertLonLat3D(Vector2 v, float radius) { return convertLonLat3D(v.x, v.y, radius); }

float mixDegree(float x, float y, float a) {
    x *= DEG2RAD;
    y *= DEG2RAD;
    Vector2 x2 = {sinf(x), cosf(x)};
    Vector2 y2 = {sinf(y), cosf(y)};
    Vector2 mixed = Vector2Normalize(Vector2Lerp(x2, y2, a));
    return atan2(mixed.x, mixed.y)*RAD2DEG;
}
