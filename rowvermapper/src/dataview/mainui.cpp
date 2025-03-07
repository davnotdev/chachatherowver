#include "mainui.hpp"

#include <iostream>

#include "imgui.h"
#include "serialin.hpp"
#include "blob.hpp"
#include "filter.hpp"
#include "ui.hpp"

MainUI::MainUI() : UI() {
    this->scanTheta = 0.0f;
    this->dataPoints = std::vector<SensorPoint>();
}

static bool liveUpdate = false;
static float cameraZoom = 2.0f;
static float measureDotX = 0.0f;
static float measureDotY = 0.0f;
static float eps = 5.0f;
static std::vector<Blob> blobs = {};
static float densityThreshold = 1.5f;
static int nearestNeighbor = 3.0;
static int selectedBlob = -1;

void MainUI::render(ImGuiIO& io) {
    bool showDemoWindow = true;
    {
        tryLoadData(this->dataPoints);

        ImGui::Begin("Info");

        ImGui::Text("Points Loaded: %d", (int)dataPoints.size());

        ImGui::SliderFloat("Camera Zoom", &::cameraZoom, 0.25f, 6.0f);
        setCameraZoom(::cameraZoom);

        ImGui::SliderFloat("Measure Dot X", &measureDotX, -1000.0f, 1000.0f);
        ImGui::SliderFloat("Measure Dot Y", &measureDotY, -1000.0f, 1000.0f);

        ImGui::SliderFloat("Blob eps", &eps, 0.0f, 40.0f);
        if (ImGui::Button("Blobs")) {
            blobs = detectBlobsByDensity(dataPoints, eps);
        }

        ImGui::Text("Blobs: %d", (int)blobs.size());

        ImGui::SliderInt("Select Blob", &selectedBlob, -1, blobs.size() - 1);

        float oldFilteringThreshold = densityThreshold;
        int oldNearestNeighbor = nearestNeighbor;
        bool applyFiltering = ImGui::Button("ReApply Filtering");
        ImGui::SliderFloat("Filtering Density Threshold", &densityThreshold, 0.1f, 2.0f);
        ImGui::SliderInt("Filtering Nearest Neighbor", &nearestNeighbor, 0, 10);
        if (applyFiltering || oldFilteringThreshold != densityThreshold || oldNearestNeighbor != nearestNeighbor) {
            this->dataPoints = filterByDensity(dataPoints, nearestNeighbor, densityThreshold);
        }

        if (ImGui::Button("Send Data")) {
            for (auto& blob : blobs) {
                std::cout 
                    << "[CMD::RES] " 
                    << blob.center.x  << ',' 
                    << blob.center.y << std::endl;
            }
            std::cout << "[CMD::END] " << std::endl;
        }

        ImGui::End();
    }

    for (auto& dataPoint: dataPoints) {
        glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::vec2 position = glm::vec2(dataPoint.x * 10.0f, dataPoint.y * 10.0f);
        
        if (selectedBlob != -1) {
            auto& points = blobs[selectedBlob].points;
            for (auto& point : points) {
                if (dataPoint.x == point.x && dataPoint.y == point.y) {
                    color = glm::vec3(0.5f, 0.0f, 0.0f);
                    break;
                }
            }
        }

        drawSquareCenter(position, glm::vec2(5.0f, 5.0f), color);
    }

    for (auto& dataPoint: blobs) {
        glm::vec2 position = glm::vec2(dataPoint.center.x * 10.0f, dataPoint.center.y * 10.0f);
        drawSquareCenter(position, glm::vec2(10.0, 10.0f), glm::vec3(1.0, 0.0, 0.0f));

        enableWireframe();
        drawSquareBounds(glm::vec2(dataPoint.minX * 10.0f, dataPoint.maxY * 10.0f), glm::vec2(dataPoint.maxX * 10.0f, dataPoint.minY * 10.0f), glm::vec3(0.0, 0.7, 0.7));
        disableWireframe();
    }

    drawSquareCenter(glm::vec2(measureDotX, measureDotY), glm::vec2(30.0, 30.0f), glm::vec3(0.0, 1.0, 0.0));
    drawSquareCenter(glm::vec2(0.0, 0.0), glm::vec2(30.0, 30.0f), glm::vec3(1.0, 0.0, 0.0));
}

