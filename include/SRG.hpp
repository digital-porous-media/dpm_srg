#ifndef SRG_HPP
#define SRG_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

// Struct to hold information about labeled regions
struct RegionInfo3D
{
    int voxelCount;
    double meanIntensity;

    RegionInfo3D() : voxelCount(0), meanIntensity(0.0) {}

    void addValue(double value)
    {
        voxelCount++;
        meanIntensity += (value - meanIntensity) / voxelCount;
    }

    double getMean() const
    {
        return meanIntensity;
    }
};

class SRG
{
public:
    SRG(const py::array_t<uint16_t> &img, const py::array_t<uint8_t> &seeds);
    ~SRG() {}

    void segment();

    py::array_t<int> getSegmentation() const;

private:
    py::array_t<uint8_t> img;
    py::array_t<uint8_t> seeds;
    std::vector<std::vector<std::vector<int>>> labels;
    std::vector<std::tuple<int, int, int>> seedPoints;
    std::vector<RegionInfo3D> regionInfos;

    std::queue<std::tuple<int, int, int>> pointQueue;

    int width, height, depth;

    const std::vector<std::tuple<int, int, int>> neighbors = {
        {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}};
    // 26-Connected is much slower and not very beneficial.
    // {-1, -1, -1}, {-1, 0, -1}, {-1, 1, -1}, {0, -1, -1}, {0, 1, -1}, {1, -1, -1},
    // {1, 0, -1}, {1, 1, -1}, {-1, -1, 0}, {1, -1, 0}, {-1, 1, 0}, {1, 1, 0},
    // {-1, -1, 1}, {-1, 0, 1}, {-1, 1, 1}, {0, -1, 1}, {0, 1, 1}, {1, -1, 1}, {1, 0, 1}, {1, 1, 1}

    void initialize();

    void processSeeds();

    bool isWithinBounds(int x, int y, int z);
};

SRG::SRG(const py::array_t<uint16_t> &img, const py::array_t<uint8_t> &seeds)
    : img(img), seeds(seeds), width(img.shape(2)), height(img.shape(1)), depth(img.shape(0))
{
    // Access pointer to np array
    py::buffer_info buf = img.request();

    if (buf.ndim != 3)
    {
        std::cerr << "Expected 3D array, but got " << buf.ndim << std::endl;
        throw std::runtime_error("Error: Expected 3D array"); // Handle the error accordingly
    }

    // Ensure the data type is correct
    if (buf.itemsize != sizeof(uint16_t))
    {
        std::cerr << "Expected int data type, but got item size: " << buf.itemsize << std::endl;
        throw std::runtime_error("Error: Incorrect data type"); // Handle the error accordingly
    }
}

void SRG::initialize()
{
    // Initialize label image
    labels.resize(width, std::vector<std::vector<int>>(height, std::vector<int>(depth, -1)));
    regionInfos.resize(std::numeric_limits<uint8_t>::max());

    // Extract seed points
    auto seedShape = seeds.shape();

    for (int x = 0; x < seedShape[0]; ++x)
    {
        for (int y = 0; y < seedShape[1]; ++y)
        {
            for (int z = 0; z < seedShape[2]; ++z)
            {
                uint8_t seedLabel = seeds.at<uint8_t>(x, y, z);
                if (seedLabel != 0)
                {
                    labels[x][y][z] = seedLabel; // Assign label
                    pointQueue.emplace(x, y, z);
                    // seedPoints.emplace_back(x, y, z); // Store seed point
                    regionInfos[seedLabel].addValue(img.at<uint16_t>(x, y, z));
                }
            }
        }
    }
}

void SRG::processSeeds()
{
    while (!pointQueue.empty())
    {
        auto [x, y, z] = pointQueue.front();
        pointQueue.pop();

        int currentLabel = labels[x][y][z];
        auto &currentRegionInfo = regionInfos[currentLabel]; // Get reference to current region info

        int mostSimilarRegionId = -1;
        double minSigma = std::numeric_limits<double>::max();
        int best_neighbor_x = -1;
        int best_neighbor_y = -1;
        int best_neighbor_z = -1;

        for (const auto &neighbor : neighbors)
        {
            int nx = x + std::get<0>(neighbor);
            int ny = y + std::get<1>(neighbor);
            int nz = z + std::get<2>(neighbor);

            if (isWithinBounds(nx, ny, nz) && labels[nx][ny][nz] == -1)
            {
                double neighborValue = img.at<uint16_t>(nx, ny, nz);
                double sigma = std::abs(neighborValue - currentRegionInfo.getMean());

                if (sigma < minSigma)
                {
                    minSigma = sigma;
                    mostSimilarRegionId = currentLabel; // Current region is the best match
                    best_neighbor_x = nx;
                    best_neighbor_y = ny;
                    best_neighbor_z = nz;
                }
            }
        }

        // If a most similar region was found, label the neighbors
        if (mostSimilarRegionId != -1)
        {
            labels[best_neighbor_x][best_neighbor_y][best_neighbor_z] = mostSimilarRegionId;

            // Update region info
            currentRegionInfo.addValue(img.at<uint16_t>(best_neighbor_x, best_neighbor_y, best_neighbor_z));

            // Add neighbors to the queue
            for (const auto &neighbor : neighbors)
            {
                int nx = best_neighbor_x + std::get<0>(neighbor);
                int ny = best_neighbor_y + std::get<1>(neighbor);
                int nz = best_neighbor_z + std::get<2>(neighbor);

                if (isWithinBounds(nx, ny, nz))
                {
                    if (labels[nx][ny][nz] == -1)
                    {
                        pointQueue.emplace(best_neighbor_x, best_neighbor_y, best_neighbor_z);
                    }
                }
            }
        }
    }
}

bool SRG::isWithinBounds(int x, int y, int z)
{
    return (x >= 0 && x < width) &&
           (y >= 0 && y < height) &&
           (z >= 0 && z < depth);
}

void SRG::segment()
{
    initialize();
    processSeeds();
}

py::array_t<int> SRG::getSegmentation() const
{
    py::array_t<int> segmented_image({depth, height, width});

    auto np_buf = segmented_image.request();
    int *np_ptr = static_cast<int *>(np_buf.ptr);

    for (size_t i = 0; i < depth; ++i)
    {
        for (size_t j = 0; j < height; ++j)
        {
            for (size_t k = 0; k < width; ++k)
            {
                np_ptr[i * height * width + j * width + k] = labels[i][j][k];
            }
        }
    }

    return segmented_image;
}

#endif
