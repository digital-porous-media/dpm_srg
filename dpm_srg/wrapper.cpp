#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "SRG.hpp"

namespace py = pybind11;

PYBIND11_MODULE(dpm_srg, m)
{
    py::class_<SRG>(m, "SRG")
        .def(py::init<const py::array_t<uint16_t> &, const py::array_t<uint8_t> &>())
        .def("segment", &SRG::segment)
        .def("get_result", &SRG::getSegmentation);
}