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

- `echo_server.py`: echo what is received
- `backdoor_server.py`: run shell commands and log back the output (both stdout
  and stderr)

> [!NOTE]  
> Note: this should support multiple clients, but logging as of now will be
> unpredictable as responses are automatically deleted upon reading.

> [!WARNING]  
> This should not be used maliciously and was just hastily coded in an hour for
> educational purposes.
