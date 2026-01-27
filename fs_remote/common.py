import os
import random

REQUESTS_DIR = '_requests'
RESULTS_DIR = '_results'

DELAY = 0.1

def get_hash():
    return hex(random.getrandbits(128))[2:]

def send_packet(dir, message):
    """
    Tries to send a packet as a file in the provided directory.
    Returns True on success, False otherwise.
    Should only fail when renaming the file due to race conditions.
    """

    ok = False
    while not ok:
        filename = get_hash()
        path = os.path.join(dir, "~" + filename)
        done_path = os.path.join(dir, filename)
        ok = not os.path.exists(path)

    with open(path, 'w', encoding='utf-8') as f:
        f.write(message)

    tries = 0
    ok = False
    while not ok and tries < 10:
        try:
            os.rename(path, done_path)
        except:
            pass
        else:
            ok = True

        tries += 1

    return ok

def receive_packet(dir):
    """
    Tries to retrieve a packet from the provided directory.
    If no file is found or an error is encountered, returns None.
    """

    for file in os.listdir(dir):
        # packet still in formation
        if file.startswith('~'):
            continue

        path = os.path.join(dir, file)

        try:
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()

            os.remove(path)

            return content
        except 1:
            pass

def common_init():
    if not os.path.isdir(REQUESTS_DIR):
        os.mkdir(REQUESTS_DIR)
    if not os.path.isdir(RESULTS_DIR):
        os.mkdir(RESULTS_DIR)
