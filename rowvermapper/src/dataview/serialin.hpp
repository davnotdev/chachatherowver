#ifndef SERIALIN_HPP
#define SERIALIN_HPP

#include <vector>
#include <thread>

/*
struct SensorRawPoint {
    long dist;
    long angle;

    bool operator==(const SensorRawPoint& other) const {
        return dist == other.dist && angle == other.angle;
    }
};
*/

struct SensorPoint {
    double x;
    double y;

    bool operator==(const SensorPoint& other) const {
        return x == other.x && y == other.y;
    }
};

std::thread initDataReader();
void terminateDataReader();
void tryLoadData(std::vector<SensorPoint>& points);

/*
std::optional<std::vector<SensorRawPoint>> serialLoadRaw(std::string filename, bool dedup = false);
std::vector<SensorPoint> serialParseRaw(std::vector<SensorRawPoint> rawPoints, int maxTheta);
*/

#endif
