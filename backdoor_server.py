import time
import subprocess as sp
from common import *

def main():
    while True:
        command = receive_packet(REQUESTS_DIR)
        if command == 'stop':
            print('Stopping server')
            send_packet(RESULTS_DIR, '[server] Successfully stopped.')
            return

        command = None if command is None else command.strip()
        if command:
            try:
                output = sp.check_output(command, stderr=sp.STDOUT).decode()
            except Exception as e:
                send_packet(RESULTS_DIR, '[server] encountered an error while\
                        executing command: %s' %e)
            else:
                send_packet(RESULTS_DIR, output)

        time.sleep(DELAY)

if __name__ == '__main__':
    common_init()
    main()
