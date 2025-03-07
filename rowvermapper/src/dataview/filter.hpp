#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include "serialin.hpp"

std::vector<SensorPoint> filterByDensity(const std::vector<SensorPoint>& points, int nearestNeighbors = 3, double densityThreshold = 1.0);

#endif
