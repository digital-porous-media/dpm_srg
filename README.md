# Seeded Region Growing

Installation instructions: ```pip install dpm_srm```

## Usage Example:
---
```
import dpm_srg
import numpy as np

np.random.seed(130621)
image = np.random.randint(0, 256, size=(100, 200, 200), dtype=np.uint16)
seeds = np.random.randint(0, 5, size=(100, 200, 200), dtype=np.uint8)

srg_obj = dpm_srg.SRG3D_u16(image, seeds)
srg_obj.segment()
segmentation = srg_obj.get_result()

## Acknowledgements:
This project includes code concepts adapted from Seeded Region Growing from ijp-toolkit, which is licensed under the LGPL-2.1 License. 

Original code can be found [here](https://github.com/ij-plugins/ijp-toolkit/tree/fe753de89a25ac1fd3aa6eb4558c6f3bf9ead7d7/src/main/java/ij_plugins/toolkit/im3d/grow)

