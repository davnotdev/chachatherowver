#include "serialin.hpp"

#include <atomic>
#include <cmath>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>

using namespace std::chrono_literals;

static std::atomic<bool> shouldTerminate; 
static std::mutex bufMutex;
static std::vector<SensorPoint> buf;

static void dataReader() {
    while (!shouldTerminate.load()) {
        std::string line;

        getline(std::cin, line);

        size_t tabPos = line.find(',');
        if (tabPos != std::string::npos) {
            std::string distStr = line.substr(0, tabPos);
            std::string angleStr = line.substr(tabPos + 1);

            int dist = std::stoi(distStr);
            int angle = std::stoi(angleStr);
            double theta = (double)(angle % 360) * (M_PI / 180.0);

            bufMutex.lock();
            buf.push_back({
                (double)dist,
                (double)angle,
                /* dist * cos(theta), */
                /* dist * sin(theta), */
            });
            bufMutex.unlock();
        }
        
    }
}

std::thread initDataReader() {
    return std::thread(dataReader);
}

void terminateDataReader() {
    shouldTerminate.store(true);
}

void tryLoadData(std::vector<SensorPoint>& points) {
    bufMutex.lock();
    for (auto& point : buf) {
        points.push_back(point);
    }
    buf.clear();
    bufMutex.unlock();
}
