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


def process_until_first_csi_line(should_print):
    """
    Processes initial serial output lines (i.e. flash/debug information) until the first CSI line is found.

    If should_print: then we print this initial serial lines.
    Otherwise: then the initial serial lines are simply flushed.
    """
    if should_print:
        print("Printing Flash information")
    while True:
        line = readline()

        if "CSI_DATA" not in line:
            if line != "" and should_print:
                print(line)
        else:
            break


def ignore_until_first_csi_line():
    """
    Ignores initial serial output lines (i.e. flash/debug information) until the first CSI line is found.
    """
    process_until_first_csi_line(should_print=False)


def print_until_first_csi_line():
    """
    Prints initial serial output lines (i.e. flash/debug information) until the first CSI line is found.
    """
    process_until_first_csi_line(should_print=True)
