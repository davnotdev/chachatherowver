#include "blob.hpp"

#include <vector>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

// Euclidean distance function
static double distance(const SensorPoint& p1, const SensorPoint& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

// Density-based blob detection function (DBSCAN-like)
std::vector<Blob> detectBlobsByDensity(const std::vector<SensorPoint>& points, double eps, int minPts) {
    std::vector<bool> visited(points.size(), false);
    std::vector<bool> isCorePoint(points.size(), false); // Tracks whether a point is a core point
    std::vector<Blob> blobs;

    // Find neighbors of a point within eps distance
    auto findNeighbors = [&](int idx) {
        std::vector<int> neighbors;
        for (size_t i = 0; i < points.size(); ++i) {
            if (distance(points[idx], points[i]) <= eps) {
                neighbors.push_back(i);
            }
        }
        return neighbors;
    };

    // Main DBSCAN-like algorithm for blob detection
    auto expandCluster = [&](int idx, std::vector<int>& neighbors, int blobIndex) {
        Blob blob;
        blob.points.push_back(points[idx]);

        double centerAccumX = points[idx].x;
        double centerAccumY = points[idx].y;

        blob.minX = points[idx].x;
        blob.maxX = points[idx].x;
        blob.minY = points[idx].y;
        blob.maxY = points[idx].y;

        // Add all neighbors of the current point
        for (size_t i = 0; i < neighbors.size(); ++i) {
            int neighborIdx = neighbors[i];
            if (!visited[neighborIdx]) {
                visited[neighborIdx] = true;
                std::vector<int> neighborNeighbors = findNeighbors(neighborIdx);

                // If the neighbor has enough points, mark it as a core point
                if (neighborNeighbors.size() >= minPts) {
                    isCorePoint[neighborIdx] = true;
                    // Add the neighbor's neighbors to the list
                    neighbors.insert(neighbors.end(), neighborNeighbors.begin(), neighborNeighbors.end());
                }
            }

            // Add point to blob
            blob.points.push_back(points[neighborIdx]);
            centerAccumX += points[neighborIdx].x;
            centerAccumY += points[neighborIdx].y;

            // Update the bounding box
            blob.minX = std::min(blob.minX, points[neighborIdx].x);
            blob.maxX = std::max(blob.maxX, points[neighborIdx].x);
            blob.minY = std::min(blob.minY, points[neighborIdx].y);
            blob.maxY = std::max(blob.maxY, points[neighborIdx].y);
        }

        // Compute the center of the blob
        glm::vec2 center = glm::vec2(centerAccumX / blob.points.size(), centerAccumY / blob.points.size());
        blob.center = center;

        // Save the blob
        blobs.push_back(blob);
    };

    // Main loop
    for (size_t i = 0; i < points.size(); ++i) {
        if (!visited[i]) {
            visited[i] = true;
            std::vector<int> neighbors = findNeighbors(i);

            if (neighbors.size() < minPts) {
                // Mark this point as noise (not enough neighbors to form a blob)
                continue;
            }

            // This is a core point, expand the cluster
            expandCluster(i, neighbors, blobs.size());
        }
    }

    return blobs;
}

