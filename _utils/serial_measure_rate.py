import sys
import time

#
# Run this utility script to view the rate at which CSI data is collected, aggregated per second.
#
# Run:
# `make monitor | python ../_utils/serial_measure_rate.py`
#

interval_start_time = 0
packet_count = 0
total_packet_counts = 0
# The first interval should not be counted (simply because it will always contain zero packets)
number_of_intervals = -1

#
# Print Flash/Debug information up until we see the first CSI packet
#
print("Printing Flash information")
while True:
    line = sys.stdin.readline()

    if "CSI_DATA" not in line:
        if line != "":
            print(line)
    else:
        break

#
# Start Evaluation
#
print("Start Evaluation")
while True:
    line = sys.stdin.readline()
    if "CSI_DATA" in line:
        if time.time() - interval_start_time > 1.0:
            number_of_intervals += 1
            if number_of_intervals > 0:
                print("Packet Count:", packet_count, "per second",
                      "Average of", total_packet_counts / number_of_intervals, "packets per second",
                      "Time:", time.time())
                interval_start_time = time.time()
                packet_count = 0
        packet_count += 1
        total_packet_counts += 1
