import time
from common import *

def main():
    print('Successfully started server.')

    while True:
        content = receive_packet(REQUESTS_DIR)
        if content == 'stop':
            print('Stopping server')
            send_packet(RESULTS_DIR, '[server] Successfully stopped.')
            return

        if content is not None:
            send_packet(RESULTS_DIR, content)

        time.sleep(DELAY)

if __name__ == '__main__':
    common_init()
    main()
