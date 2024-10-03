import numpy as np
from build.dpm_srg import SRG2D_u8
import matplotlib.pyplot as plt
import tifffile
import time

image_data = np.fromfile("/mnt/c/Users/bcc2459/Documents/srm_test_data/image_data.raw", dtype=np.uint16).reshape((256, 256, 256))
seed_data = np.fromfile("/mnt/c/Users/bcc2459/Documents/srm_test_data/seeds.raw", dtype=np.uint8).reshape((256, 256, 256))

image_data = np.ascontiguousarray(image_data)
seed_data = np.ascontiguousarray(seed_data)

min_val = np.percentile(image_data, 1)
max_val = np.percentile(image_data, 99)
image_data[image_data < min_val] = min_val
image_data[image_data > max_val] = max_val
image_data = 255 * (image_data - min_val) / (max_val - min_val)

image_data = image_data[50]
seed_data = seed_data[50]
image_data = image_data.astype(np.uint8)

# seed_data[seed_data == 2] = 0
# seed_data[seed_data == 3] = 2
# Create SRG object
# image_data = image_data[:128, :200, :]
# seed_data = seed_data[:128, :200, :]

srg_obj = SRG2D_u8(image_data, seed_data)
tik = time.perf_counter_ns()
# Perform segmentation
srg_obj.segment()


# Get segmentation result
segmentation = srg_obj.get_result()
print(f"Elapsed time: {(time.perf_counter_ns() - tik)*1e-9:.3f} s")

# plt.imshow(image_data[50], cmap='Greys_r')
# plt.figure()
# plt.imshow(seed_data[50], cmap="Greys_r")
# plt.figure()
# plt.imshow(segmentation[50], cmap='Greys_r')
# plt.show()

plt.imshow(image_data, cmap='Greys_r')
plt.figure()
plt.imshow(seed_data, cmap="Greys_r")
plt.figure()
plt.imshow(segmentation, cmap='Greys_r')
plt.show()

