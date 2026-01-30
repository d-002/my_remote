# my_remote

Tool suite for remote shells and whatnot

> [!WARNING]  
> This code should not be used maliciously and is there for educational
> purposes, to provide insights for detection development, as well as show some
> consequences of a bad usage of things like a shared file storage.
> I am only doing this to research different technologies (php, machines
> communication, C file descriptors...)
> 
> Do not use this code maliciously, namely to attack or scan real devices.
> Unauthorized use is illegal and violates GitHub policy.

## fs_remote

A basic client/server protocol using a file system for communication.  
Allows things like a basic remote shell, and easily allows more features to be
added on top of it.

**Requirements:**

- Python 3.x
- Python libraries for the client: `prompt_toolkit`

## shell_server

A simple remote shell, operated from a web interface.
Try it on your own machine! (trust)

**Requirements:**

- A PHP-capable server, that supports sqlite3
- C compiling suite on the target machines

**Setup:**

- Host the server.
- Log into the server, creating a user to which the machines you affect will be
  linked.
  This also adds a small security layer to determine who is able to run commands
  on these machines.
- Run the remote shell installer on the target machine, which will query for the
  url to your server, as well as your login information.
  This registers the client and makes it visible in your dashboard.

The remote shell program sends a heartbeat to the server, which also serves as
a way to check its software version.
Any new version is automatically installed on the targets by querying the code
on the server, updating the local files and restarting the program.
Software versions are handled separately for all website users, to avoid
breaking changes.

**Technical details:**

On creating an account on the server a unique hash is associated with it.
When setting up the remote shell on a machine, upon logging the server answers
with two keys: the key of the user, and a new key for the machine.
On the server side, the machine's key is added to the user's linked machines.

The key pair will be used during transactions to identify the machine and the
user it wants to communicate with.
The client (and in some cases the website user) initiates all data
transmissions: querying for a new version or update files, to read the command
queue and edit it, or write to the output stream (combined stdout and stderr).

Server admins have full control and can view all keys but this should not really
matter (and I'm too lazy to fix this).
