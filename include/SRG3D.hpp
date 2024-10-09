/*
   This file is adapted from the grow module in ijp-toolkit, which is licensed under GNU Lesser General Public License v2.1

    Seeded Region Growing Segmentation
    Copyright (C) 2024  Digital Porous Media

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SRG3D_HPP
#define SRG3D_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>
#include "SRG.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

template <typename T>
class SRG3D : public SRG<T, 3>
{
public:
    SRG3D(const py::array_t<T> &image, const py::array_t<uint8_t> &seeds);
    ~SRG3D() {}

    py::array_t<uint8_t> getSegmentation() const override;

private:
    // const py::array_t<T> image;
    const T *img_ptr;
    const uint8_t *seeds_ptr;
    // const py::array_t<uint8_t> seeds;
    const uint16_t width, height, depth;

    std::vector<std::vector<std::vector<int>>> labels;
    std::vector<std::tuple<int, int, int>> seedPoints;
    std::vector<RegionInfo> regionInfos;

    std::queue<std::tuple<int, int, int>> pointQueue;

    const std::vector<std::tuple<int, int, int>> neighbors = {
        {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}};

    void initialize() override;

    void processSeeds() override;

    bool isWithinBounds(int x, int y, int z);
};

template <typename T>
SRG3D<T>::SRG3D(const py::array_t<T> &image, const py::array_t<uint8_t> &seeds)
    : width(image.shape(2)), height(image.shape(1)), depth(image.shape(0))
{
    // Access pointer to np array
    py::buffer_info buf = image.request();

    if (buf.ndim != 3)
    {
        std::cerr << "Expected 3D array, but got " << buf.ndim << std::endl;
        throw std::runtime_error("Error: Expected 3D array"); // Handle the error accordingly
    }

    // Ensure the data type is correct
    if (buf.itemsize != sizeof(T))
    {
        std::cerr << "Expected int data type, but got item size: " << buf.itemsize << std::endl;
        throw std::runtime_error("Error: Incorrect data type"); // Handle the error accordingly
    }

    img_ptr = static_cast<const T *>(buf.ptr);

    if (!img_ptr)
    {
        std::cerr << "img_ptr is null!" << std::endl;
        throw std::runtime_error("Error: img_ptr is null!"); // or handle the error appropriately
    }

    seeds_ptr = static_cast<const uint8_t *>(seeds.request().ptr);

    if (!seeds_ptr)
    {
        std::cerr << "seeds_ptr is null!" << std::endl;
        throw std::runtime_error("Error: seeds_ptr is null!"); // or handle the error appropriately
    }
}

template <typename T>
void SRG3D<T>::initialize()
{
    // Initialize label image
    labels.resize(width, std::vector<std::vector<int>>(height, std::vector<int>(depth, -1)));
    regionInfos.resize(std::numeric_limits<uint8_t>::max());

    // Extract seed points
    for (size_t x = 0; x < width; ++x)
    {
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t z = 0; z < depth; ++z)
            {
                uint8_t seedLabel = seeds_ptr[x + y * width + z * width * height];

                if (seedLabel != 0)
                {
                    labels[x][y][z] = seedLabel; // Assign label
                    pointQueue.emplace(x, y, z);
                    // seedPoints.emplace_back(x, y, z); // Store seed point
                    regionInfos[seedLabel].addValue(img_ptr[x + y * width + z * width * height]);
                }
            }
        }
    }
}

template <typename T>
void SRG3D<T>::processSeeds()
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
                double neighborValue = static_cast<double>(img_ptr[nx + ny * width + nz * height * width]);
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
            currentRegionInfo.addValue(img_ptr[best_neighbor_x + best_neighbor_y * width + best_neighbor_z * height * width]);

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

template <typename T>
bool SRG3D<T>::isWithinBounds(int x, int y, int z)
{
    return (x >= 0 && x < width) &&
           (y >= 0 && y < height) &&
           (z >= 0 && z < depth);
}

template <typename T>
py::array_t<uint8_t> SRG3D<T>::getSegmentation() const
{
    py::array_t<uint8_t> segmented_image = py::array_t<uint8_t>({depth, height, width});

    pybind11::buffer_info np_buf = segmented_image.request();
    uint8_t *np_ptr = static_cast<uint8_t *>(np_buf.ptr);

    for (size_t i = 0; i < depth; ++i)
    {
        for (size_t j = 0; j < height; ++j)
        {
            for (size_t k = 0; k < width; ++k)
            {
                np_ptr[i * height * width + j * width + k] = static_cast<uint8_t>(labels[k][j][i]);
            }
        }
    }

    return segmented_image;
}

#endif
