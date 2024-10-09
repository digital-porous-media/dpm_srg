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
struct RegionInfo
{
    int voxelCount;
    double meanIntensity;

    RegionInfo() : voxelCount(0), meanIntensity(0.0) {}
    template <typename T>
    void addValue(T value)
    {
        voxelCount++;
        meanIntensity += (static_cast<double>(value) - meanIntensity) / voxelCount;
    }

    double getMean() const
    {
        return meanIntensity;
    }
};

template <typename T, int Dimensions>
class SRG
{
public:
    virtual ~SRG() {}

    virtual void segment();

    virtual py::array_t<int> getSegmentation() const = 0;

protected:
    std::vector<RegionInfo> regionInfos;

    virtual void initialize() = 0;

    virtual void processSeeds() = 0;
};

template <typename T, int Dimensions>
void SRG<T, Dimensions>::segment()
{
    initialize();
    processSeeds();
}

#endif