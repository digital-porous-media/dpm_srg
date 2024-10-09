from setuptools import setup, Extension
import pybind11
from pybind11.setup_helpers import Pybind11Extension

# Define your extension module
ext_modules = [
    Pybind11Extension(
        'dpm_srg',  # Name of the module
        ['wrappers/wrapper.cpp'],
        include_dirs=[pybind11.get_include(), './include'],
        language='c++'
    ),
]

setup(
    name='dpm_srg',
    version='0.0.1',
    author='Digital Porous Media',
    author_email='bcchang@utexas.edu',
    description='Seeded Region Growing Segmentation',
    ext_modules=ext_modules,
    zip_safe=False,
    packages=find_packages(exclude=["include"])
)
