from time import time

class WaitTimer():
    """
    Simple implementation for only performing a task once every `wait_time_seconds` seconds.
    """
    def __init__(self, wait_time_seconds):
        self.wait_time_seconds = wait_time_seconds
        self.interval_start_time = time()

    def check(self):
        # Check if we have waited for `wait_time_seconds`
        return time() - self.interval_start_time > self.wait_time_seconds

    def update(self):
        # Update the start time of our timer.
        self.interval_start_time = time()