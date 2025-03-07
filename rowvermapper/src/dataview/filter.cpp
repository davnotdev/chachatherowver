#include "filter.hpp"

#include <vector>
#include <cmath>
#include <algorithm>

constexpr int kNearestNeighbors = 3;
constexpr double kDensityThreshold = 1.0;

// Compute the mean of a list of SensorPoint points
SensorPoint computeMean(const std::vector<SensorPoint>& points) {
    SensorPoint sum{0.0, 0.0};
    for (const auto& p : points) {
        sum.x += p.x;
        sum.y += p.y;
    }
    return {sum.x / points.size(), sum.y / points.size()};
}

// Compute the standard deviation of points
SensorPoint computeStdDev(const std::vector<SensorPoint>& points, const SensorPoint& mean) {
    SensorPoint variance{0.0, 0.0};
    for (const auto& p : points) {
        variance.x += (p.x - mean.x) * (p.x - mean.x);
        variance.y += (p.y - mean.y) * (p.y - mean.y);
    }
    return {std::sqrt(variance.x / points.size()), std::sqrt(variance.y / points.size())};
}

// Compute Euclidean distance between two points
static double distance(const SensorPoint& a, const SensorPoint& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Compute density for each point and filter based on a threshold
std::vector<SensorPoint> filterByDensity(const std::vector<SensorPoint>& points, int nearestNeighbors, double densityThreshold) {
    if (points.size() < nearestNeighbors + 1) return points;
    
    std::vector<SensorPoint> filtered;
    for (const auto& p : points) {
        std::vector<double> distances;
        for (const auto& q : points) {
            if (&p != &q) {
                distances.push_back(distance(p, q));
            }
        }
        std::sort(distances.begin(), distances.end());
        double avgDistance = 0.0;
        for (int i = 0; i < nearestNeighbors; ++i) {
            avgDistance += distances[i];
        }
        avgDistance /= nearestNeighbors;
        double density = 1.0 / (avgDistance + 1e-6);
        if (density >= densityThreshold) {
            filtered.push_back(p);
        }
    }
    return filtered;
}

