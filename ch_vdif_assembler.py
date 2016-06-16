import os
import re
import sys
import errno
import numpy as np

import ch_vdif_assembler_cython


class constants:
    chime_nfreq = ch_vdif_assembler_cython.chime_nfreq                      # 1024
    timestamps_per_frame = ch_vdif_assembler_cython.timestamps_per_frame    # 2^23 (cadence of noise source)
    num_disks = ch_vdif_assembler_cython.num_disks                          # 10 (moose)