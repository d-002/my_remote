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
Allows things like a basic remote shell, and more things like client
identification could easily be added on top of it.

**Requirements:**

- Python 3.x
- Client: Python libraries: `prompt_toolkit`
- Server: Python libraries: nothing

## 

A simple remote shell, operated from a web interface.
Try it on your own machine! (trust)

**Requirements:**

- A PHP-capable server
- Python 3 on the target machine

**Setup:**

- Host the server.
- Log into the server, creating a user to which the machines you affect will be
  linked.
  This also adds a small security layer for who is able to run commands on these
  machines.
- Run the remote shell installer on the target machine, which will query for the
  url to your server, as well as your login information to make sure you are
  who you pretend to be.
  The code will then store this encrypted information for authentication during
  communication as well as download the remote shell program from the server.

The remote shell program sends a heartbeat to the server, which also serves as
a way to check its software version.

Any new version is automatically installed by updating the code and restarting
the program.

If you are authorized, you can then access a shell and send commands to a
specific client.
The commands are queued, even if the client is offline.

**Technical details:**

The login details are encrypted client-side on top of the server-side
encryption, to provide security even on http servers.

As for server / client authorization:

On creating an account on the server a unique hash is associated with it.
When setting up the remote shell on a machine, upon logging the server answers
with two keys: first the key of the user, then a new key for the machine.
On the server-side, the machine's key is added to the list linked to the user.

The key pair will be used during transactions to identify the machine and the
user it wants to communicate with.
The client initiates all data transmissions: querying for a new version, updated
program files, the command queue, or for a dequeue instruction.

Server admins have full control and can view all keys but this should not really
matter (and I'm too lazy to fix this).
