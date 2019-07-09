import sys
import time

#
# Run this utility to append the current real timestamp.
#
# Run:
# `make monitor | python ../_utils/serial_append_timestamp.py`
#

#
# Append the first line with ",timestamp"
#
while True:
    line = sys.stdin.readline()

    if "CSI_DATA" in line:
        l = line.rstrip()
        print(line.rstrip() + ",timestamp")
        break

#
# Append subsequent lines with the current timestamp
#
while True:
    line = sys.stdin.readline()

    if "CSI_DATA" in line:
        print(line.rstrip() + "," + str(time.time()))
