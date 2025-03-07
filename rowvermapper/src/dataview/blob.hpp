#ifndef BLOB_HPP
#define BLOB_HPP

#include <vector>
#include <glm/glm.hpp>
#include "serialin.hpp"

struct Blob {
    std::vector<SensorPoint> points;
    glm::vec2 center;
    double minX, maxX, minY, maxY;
};

std::vector<Blob> detectBlobsByDensity(const std::vector<SensorPoint>& points, double eps, int minPts = 3);

#endif

