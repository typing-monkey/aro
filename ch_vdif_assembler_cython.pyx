from libc.stdint cimport int32_t, int64_t
from libcpp.vector cimport vector
from libcpp cimport bool

import numpy as np
cimport numpy as np

cimport ch_vdif_assembler_pxd

chime_nfreq = ch_vdif_assembler_pxd.number_of_channels
timestamps_per_frame = ch_vdif_assembler_pxd.timestamps_per_frame