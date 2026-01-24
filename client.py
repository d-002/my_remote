import sys
import time
import asyncio
from prompt_toolkit import PromptSession
from prompt_toolkit.patch_stdout import patch_stdout
from common import *

PROMPT = 'my_remote: '

run_listener = True

async def listener():
    while run_listener:
        content = receive_packet(RESULTS_DIR)
        if content is not None:
            print(content)

        await asyncio.sleep(DELAY)

async def main():
    session = PromptSession()
    asyncio.create_task(listener())

    while True:
        with patch_stdout():
            try:
                payload = await session.prompt_async(PROMPT)

                ok = send_packet(REQUESTS_DIR, payload)
                if not ok:
                    print('Error sending the request')

            except KeyboardInterrupt:
                print('Keyboard interrupt, stopping listener...')
                run_listener = False
                return

if __name__ == '__main__':
    common_init()

    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
