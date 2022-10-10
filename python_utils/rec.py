import sys
import matplotlib.pyplot as plt
import math
import numpy as np

perm_amp = []
perm_phase = []
# Create figure for plotting
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
count = 0
plt.xlabel("X-axis")
plt.ylabel("Y-axis")
plt.title("Updating plot...")
fig.canvas.draw()
plt.show(block=False)
process_line = True


def carrier_plot(amp):

    #Adds xlims after 100 entries
    if count > 100:
        plt.xticks([], [])
        plt.xlim()

    else:
        plt.xlim(0, 100)
    plt.xlabel("Packet number/Time")
    plt.ylabel("Amplitude")
    plt.rcParams["figure.figsize"] = [7.50, 3.50]
    plt.rcParams["figure.autolayout"] = True
    plt.title("Line graph")
    plt.show()

    df = np.asarray(amp)
    # print(df.shape)
    # Can be changed to df[x] to plot sub-carrier x only (set color='r' also)
    plt.plot(df[:, 44], color='r')
    # fig.canvas.draw()

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
    # if process_line:    # Only process every other line?
    if "CSI_DATA" in line:
        count += 1
        process(line)
        print("Number of lines read: " + str(count))

        if count > 5:
            if len(perm_amp) < 100:
                carrier_plot(perm_amp)
            else:
                carrier_plot(perm_amp[len(perm_amp)-100:len(perm_amp)])
                # carrier_plot(perm_amp)
    # process_line = not process_line
