import sys
import matplotlib.pyplot as plt
import math
import numpy as np
import collections

# Set subcarrier to plot
subcarrier = 44

# Deque definition
perm_amp = collections.deque(maxlen=100)
perm_phase = collections.deque(maxlen=100)

# Create figure for plotting
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
fig.canvas.draw()
plt.show(block=False)


def readline():
    while True:
        try:
            sys.stdin.buffer.flush()
            return sys.stdin.buffer.readline().decode('utf-8').replace("\n", "")
        except:
            pass  # might not be a utf-8 string!


def carrier_plot(amp):
    plt.clf()
    df = np.asarray(amp, dtype=np.int32)
    # Can be changed to df[x] to plot sub-carrier x only (set color='r' also)
    plt.plot(range(100-len(amp), 100), df[:, subcarrier], color='r')
    plt.xlabel("Time")
    plt.ylabel("Amplitude")
    plt.xlim(0, 100)
    plt.title(f"Amplitude plot of Subcarrier {subcarrier}")
    # TODO use blit instead of flush_events for more fastness
    # to flush the GUI events
    fig.canvas.flush_events()
    plt.show()


def process(res):
    # Parser
    all_data = res.split(',')
    csi_data = all_data[25].split(" ")
    csi_data[0] = csi_data[0].replace("[", "")
    csi_data[-1] = csi_data[-1].replace("]", "")

    csi_data.pop()
    csi_data = [int(c) for c in csi_data if c]
    imaginary = []
    real = []
    for i, val in enumerate(csi_data):
        if i % 2 == 0:
            imaginary.append(val)
        else:
            real.append(val)

    csi_size = len(csi_data)
    amplitudes = []
    phases = []
    if len(imaginary) > 0 and len(real) > 0:
        for j in range(int(csi_size / 2)):
            amplitude_calc = math.sqrt(imaginary[j] ** 2 + real[j] ** 2)
            phase_calc = math.atan2(imaginary[j], real[j])
            amplitudes.append(amplitude_calc)
            phases.append(phase_calc)

        perm_phase.append(phases)
        perm_amp.append(amplitudes)

count = 0
while True:
    line = readline()
    if "CSI_DATA" in line:
        count += 1
        process(line)
        print("Number of lines read: " + str(count))
        carrier_plot(perm_amp)
