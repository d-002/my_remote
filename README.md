# my_remote
hopefully reliable interface for a remote shell without any ip nonsense

## Requirements

- Python 3.x
- Client: `prompt_toolkit`
- Server: nothing

## Setup

On the server side, pick a server file (ex `backdoor_server.py`) and run it with
Python.

On the client side, go to the same shared directory and run the client:
`python3 client.py`.

You can now type commands on the client side and get back the server's output.

For `echo_server.py`, this test servers echoes what it receives.

For `backdoor_server.py`, this test runs shell commands in a new process and
logs back the merged outputs of both stdout and stderr.

> [!INFO]  
> Note: this should support multiple clients, but logging as of now will be
> unpredictable as responses are automatically deleted upon reading.
