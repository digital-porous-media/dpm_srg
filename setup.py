from setuptools import setup, find_packages
import pybind11
from pybind11.setup_helpers import Pybind11Extension
import sys
from pathlib import Path

if sys.platform == "win32":
    extra_compile_args = ["/std:c++17"]
else:
    extra_compile_args = ["-std=c++17"]

this_directory = Path(__file__).parent
long_description = (this_directory/"README.md").read_text()

# Define your extension module
ext_modules = [
    Pybind11Extension(
        'dpm_srg',
        ['wrappers/wrapper.cpp'],
        include_dirs=[pybind11.get_include(), './include'],
        language='c++',
        extra_compile_args=extra_compile_args
    ),
]

setup(
    name='dpm_srg',
    version='0.0.3',
    author='Digital Porous Media',
    author_email='bcchang@utexas.edu',
    description='Seeded Region Growing Segmentation',
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=ext_modules,
    zip_safe=False,
    packages=find_packages(exclude=["include"])
)
