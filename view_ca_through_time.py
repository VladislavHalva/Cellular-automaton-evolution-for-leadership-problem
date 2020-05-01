#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt

if (len(sys.argv) == 2):
    filename = sys.argv[1]

    f = open(filename,"r")
    time_diag_text = f.read()

    time_diag = [[]]
    for i in range(len(time_diag_text)):
        if time_diag_text[i] == '\n':
            time_diag.append([])
        elif time_diag_text[i] == ' ':
            continue
        else:
            time_diag[-1].append(int(time_diag_text[i]))

    fig = plt.figure(figsize=(50, 100))
    plt.imshow(time_diag[:450])
    plt.xlabel("Buňky celulárního automatu")
    plt.ylabel("Čas (diskrétní kroky automatu)")
    plt.title("Ukázka vývoje celulárního automatu řešícího problém volby vůdce v diskrétních časových krocích. (Žlutá barva = 1, Fialová = 0)")
    plt.show()