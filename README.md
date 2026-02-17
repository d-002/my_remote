# my_remote

Tool suite for remote shells and whatnot

> [!WARNING]  
> This code should not be used maliciously and is there for educational
> purposes, to provide insights for detection development, as well as show some
> consequences of a bad usage of things like a shared file storage.
> I am only doing this to research different technologies (php, socket
> communication in C...)
> 
> Do not use this code maliciously, namely to attack or scan real devices.
> Unauthorized use is illegal and violates GitHub policy.

## fs_remote

A basic client/server protocol using a file system for communication.  
Allows things like a basic remote shell and features could easily be added on
top of it.

**Requirements:**

- Python 3.x
- Python libraries for the client: `prompt_toolkit`

## shell_server

A simple remote shell, operated from a web interface.
Try it on your own machine! (trust)

**Requirements:**

- A PHP-capable server, that supports sqlite3
- C compiling suite on the target machines in case the auto-updates using static
  executables is not desired, as well as Python 3 for the installer
- For building when using the auto-updates system, `musl-gcc`
- For the installer, Python 3.x

**Setup:**

- Host the server.
  You will also need to create files `/software/version` (text containing the
  version) and `/software/binary` (compiled static software) for when users are
  created.
- Log into the server, creating a user to which the machines you affect will be
  linked.
  This also adds a small security layer to determine who is able to run commands
  on these machines.
- Run the remote shell installer on the target machine, which will query for the
  url to your server, as well as your login information.
  This registers and launches the client and makes it visible in your dashboard.

The remote shell program sends a heartbeat to the server, which also serves as
a way to check its software version.
If you choose to enable automatic software updates, you can provide a compiled
executable in your dashboard and remote shells will automatically update to that
provided version.
Software versions are handled separately for all website users, to avoid
breaking changes.

**Technical details:**

In systems where you do not wish to use the auto-update system, for example if
you want to compile the software yourself on a machine and never override it,
you will need to say so during the installation process with the installer, as
well as place the desired binary in the provided directory.

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
Additionally, as data is not encrypted you are advised to use complementary
end-to-end encryption if safety is a concern.
