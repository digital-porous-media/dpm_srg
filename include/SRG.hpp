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

    void addValue(auto value)
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