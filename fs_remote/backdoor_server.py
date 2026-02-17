import time
import subprocess
from common import *

def main():
    print('Successfully started server.')

    while True:
        command = receive_packet(REQUESTS_DIR)
        if command == 'stop':
            print('Stopping server')
            send_packet(RESULTS_DIR, '[server] Successfully stopped.')
            return

        command = None if command is None else command.strip()
        if command:
            try:
                output = subprocess.run(command,
                                        shell=True,
                                        capture_output=True,
                                        text=True)
                output = ''.join(elt for elt in [output.stdout, output.stderr] if elt)
            except Exception as e:
                send_packet(RESULTS_DIR, '[server] encountered an error while\
                        executing command: %s' %e)
            else:
                send_packet(RESULTS_DIR, output)

        time.sleep(DELAY)

if __name__ == '__main__':
    common_init()

    try:
        main()
    except KeyboardInterrupt:
        print('Keyboard interrupt')
