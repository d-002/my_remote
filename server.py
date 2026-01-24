from common import *

common_init()

send_packet(REQUESTS_DIR, "hello world")

while True:
    content = receive_packet(REQUESTS_DIR)
    if content is None:
        break

    print(content)
