import sys
import matplotlib.pyplot as plt
import math
import numpy as np
import collections

#Que definition
perm_amp = collections.deque(maxlen=100)
perm_phase = collections.deque(maxlen=100)
#Set subcarrier to plot
Subcarrier=44


# Create figure for plotting
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
count = 0

#Plot definitions
plt.xlabel("Time")
plt.ylabel("Amplitude")
plt.title(f"Amplitude plot of Subcarrier {Subcarrier}")
fig.canvas.draw()
plt.show(block=False)
process_line = True
plt.xlim(0, 100)
plt.rcParams["figure.figsize"] = [7.50, 3.50]
plt.rcParams["figure.autolayout"] = True


def carrier_plot(amp):

    #Adds xlims after 100 entries
    plt.show()
    df = np.asarray(amp,dtype=np.int32)
    # Can be changed to df[x] to plot sub-carrier x only (set color='r' also)
    plt.plot(df[:,Subcarrier], color='r')
    # TODO use blit instead of flush_events for more fastness
    # to flush the GUI events
    fig.canvas.flush_events()
    plt.clf()


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


while True:
    line = sys.stdin.readline()
    # if process_line:    # Only process every other line
    if "CSI_DATA" in line:
        count += 1
        process(line)
        print("Number of lines read: " + str(count))
        if count > 5:
            carrier_plot(perm_amp)



