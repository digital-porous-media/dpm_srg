import numpy as np
from build.dpm_srg import SRG
import matplotlib.pyplot as plt
import tifffile
import time

image_data = np.fromfile("/mnt/c/Users/bcc2459/Documents/dpm_srg/data/image_data.raw", dtype=np.uint16).reshape((256, 256, 256))
seed_data = np.fromfile("/mnt/c/Users/bcc2459/Documents/dpm_srg/data/seeds.raw", dtype=np.int8).reshape((256, 256, 256))

seed_data[seed_data == 2] = 0
seed_data[seed_data == 3] = 2
# Create SRG object
srg = SRG(image_data, seed_data)
tik = time.perf_counter_ns()
# Perform segmentation
srg.segment()


# Get segmentation result
segmentation = srg.get_result()
print(f"Elapsed time: {(time.perf_counter_ns() - tik)*1e-9:.3f} s")

plt.imshow(image_data[50], cmap='Greys_r')
plt.figure()
plt.imshow(seed_data[50], cmap="Greys_r")
plt.figure()
plt.imshow(segmentation[50], cmap='Greys_r')
plt.show()


