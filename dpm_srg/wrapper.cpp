#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "SRG.hpp"
#include "SRG3D.hpp"
#include "SRG2D.hpp"

namespace py = pybind11;

// Template function to help wrap SRG3D with different datatypes
template <typename T>
void wrap_srg3d(py::module &m, const std::string &suffix)
{
    std::string class_name = "SRG3D_" + suffix;
    py::class_<SRG3D<T>>(m, class_name.c_str())
        .def(py::init<const py::array_t<T> &, const py::array_t<uint8_t> &>(),
             py::arg("image"), py::arg("seeds"))
        .def("segment", &SRG3D<T>::segment)
        .def("get_result", &SRG3D<T>::getSegmentation);
}

template <typename T>
void wrap_srg2d(py::module &m, const std::string &suffix)
{
    std::string class_name = "SRG2D_" + suffix;
    py::class_<SRG2D<T>>(m, class_name.c_str())
        .def(py::init<const py::array_t<T> &, const py::array_t<uint8_t> &>(),
             py::arg("image"), py::arg("seeds"))
        .def("segment", &SRG2D<T>::segment)
        .def("get_result", &SRG2D<T>::getSegmentation);
}

PYBIND11_MODULE(dpm_srg, m)
{
    m.doc() = "Seeded Region Growing (SRG) Segmentation module";
    wrap_srg3d<uint8_t>(m, "u8");
    wrap_srg3d<uint16_t>(m, "u16");
    wrap_srg3d<int8_t>(m, "i8");
    wrap_srg3d<int16_t>(m, "i16");
    wrap_srg2d<uint8_t>(m, "u8");
    wrap_srg2d<uint16_t>(m, "u16");
    wrap_srg2d<int8_t>(m, "i8");
    wrap_srg2d<int16_t>(m, "i16");
}
