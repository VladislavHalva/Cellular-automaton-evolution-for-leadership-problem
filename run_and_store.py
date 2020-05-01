#!/usr/bin/env python3

import os
import sys
import numpy as np
from tqdm import tqdm

if(len(sys.argv) == 3):
    for i in tqdm(range(int(sys.argv[1]))):
        stream = os.popen('./ca_evol_leadership')
        output = stream.read() 

        f = open(sys.argv[2], "a")
        f.write(output)

