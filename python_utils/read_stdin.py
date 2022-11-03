import sys


def readline():
    """
    Ignores errors when reading lines from stdin.
    """
    while True:
        try:
            sys.stdin.buffer.flush()
            return sys.stdin.buffer.readline().decode('utf-8').replace("\n", "")
        except:
            pass  # might not be a utf-8 string!


def print_until_first_csi_line():
    """
    Prints initial serial output lines (i.e. flash/debug information) until the first CSI line is found.
    """
    print("Printing Flash information")
    while True:
        line = readline()

        if "CSI_DATA" not in line:
            if line != "":
                print(line)
        else:
            break
