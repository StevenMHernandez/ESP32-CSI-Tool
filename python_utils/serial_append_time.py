import sys
import time
from read_stdin import readline, print_until_first_csi_line

#
# Run this utility to append the current real timestamp.
#
# Run:
# `idf.py monitor | python ../python_utils/serial_append_timestamp.py`
#

print_until_first_csi_line()

#
# Append subsequent lines with the current timestamp
#
while True:
    line = readline()

    if "CSI_DATA" in line:
        print(line.strip() + "," + str(time.time()))
