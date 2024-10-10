# Seeded Region Growing

Seeded Region Growing (SRG) is a method for image segmentation. It involves selecting initial seed points and expanding these regions by adding neighboring pixels that share similar properties, such as grayscale intensity. The process continues iteratively until the entire image is segmented into distinct labels. 

This library is an adaptation of the [SRG plugin to Fiji/ImageJ](https://ij-plugins.sourceforge.net/plugins/segmentation/srg.html) and is based on the algorithm proposed in [Adams and Bischof](10.1109/34.295913). Our contribution was to translate the original algorithm to C++ and wrap it in Python. We provide this package under the Digital Porous Media organization.

## Installation:
dpm_srg is packaged on [pypi](https://pypi.org/project/dpm-srg/) and can be installed with pip.
```pip install dpm-srg```.

If installing from source, this package requires a C++ compiler.

## Usage Example:
---
This implementation of SRG expects a 2D or 3D grayscale (single color channel) image of type uint8, uint16, or uint32, and a seed image of the same shape as the grayscale image and of type uint8. All seeds should be labeled between 1-255, inclusive as the algorithm fills in points in the seeded image that are labeled 0. The algorithm will return a labeled image of the same shape as the input images of type uint8. Therefore, up to 255 unique labels are possible.

Note that the performance and segmentation accuracy is highly sensitive to the selection of seed points. It is generally better to take a more conservative approach when selecting seed points.

We wrapped each version (2D vs. 3D, dtype) of the template class into individual class instances. The nomenclature is: SRG[2(or 3)]D_u[number_of_bits]() (e.g. ```SRG2D_u8()```, ```SRG3D_u32()```).

**Python Example:**
```
import dpm_srg
import numpy as np

np.random.seed(130621)
image = np.random.randint(0, 256, size=(100, 200, 200), dtype=np.uint16)
seeds = np.random.randint(0, 5, size=(100, 200, 200), dtype=np.uint8)

srg_obj = dpm_srg.SRG3D_u16(image, seeds)
srg_obj.segment()
segmentation = srg_obj.get_result()
```

## Acknowledgements:
This project includes code concepts adapted from Seeded Region Growing from ijp-toolkit, which is licensed under the LGPL-2.1 License. 

Original code can be found [here](https://github.com/ij-plugins/ijp-toolkit/tree/fe753de89a25ac1fd3aa6eb4558c6f3bf9ead7d7/src/main/java/ij_plugins/toolkit/im3d/grow)

