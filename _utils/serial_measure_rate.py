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
packet_counts_list = []

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
                neg_index = line.find("-")
                rssi = line[neg_index:neg_index+4]
                packet_counts_list.append(packet_count)
                packet_counts_list = packet_counts_list[-11:]
                print("Packet Count:", packet_count, "           per second",
                      "Average of", total_packet_counts / number_of_intervals, "packets per second",
                      "Average of (last 10)", sum(packet_counts_list) / len(packet_counts_list), "packets per second",
                      "Time:", time.time(),
                      "RSSI:", rssi)
                interval_start_time = time.time()
                # print(packet_counts_list)
                packet_count = 0
        packet_count += 1
        total_packet_counts += 1
