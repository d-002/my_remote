# my_remote

Tool suite for remote shells and whatnot

> [!WARNING]  
> This code should not be used maliciously and is there for educational
> purposes, to provide insights for detection development, as well as show some
> consequences of a bad usage of things like a shared file storage.
> I am only doing this to research different technologies (php, socket
> communication in C, custom API protocols, command and control study...)
> 
> Do not use this code maliciously, namely to attack, scan or keep control over
> real devices.
> Unauthorized use is illegal and violates GitHub policy.
> 
> The author is not responsible for any misuse of this software.
> By downloading or using this code, you agree to take full responsibility for
> your actions and to comply with all local, state, and federal laws, as well as
> institutional policies.

## fs_remote

A basic, lightweight protocol for sending commands remotely over a shared file
system.

This is a study of data-exfiltration and C2 channel via shared storage.

**Requirements:**

- Python 3.x
- Python libraries for the client: `prompt_toolkit`

## shell_server

A simple remote shell that can be operated namely from a web (PHP) interface.

![screenshot of the shell_server dashboard][1]

This is study for a centralized, scalable Command and control framework, with
automated provisioning, version control, authentication system with accounts
and concurrent users handling.

For example, multiple users can manage an endpoint without any of them knowing
about the other users, all while accessing the same remote control panel.

Send commands remotely to a machine, have a real-time feedback on their output,
update the software, shut it down, reasonably reduce its footprint remotely.

**Tech stack:**

- Backend: PHP 8.x + sqlite3
- Client: C
- Toolchain: `musl-gcc` for portability over Linux-based systems, but the usage
  of a custom binary is possible on installation (if you want to run the program
  on Windows for example).
- Client installer: Python 3.x

**Setup:**

- Host the server.
  You will also need to create files `/software/version` (text containing the
  version) and `/software/binary` (compiled static software) for when users are
  created.
  Currently, this central software and its associated version can only be
  changed manually by server administrators.
- Log into the server, creating a user to which the remote agents will be
  linked.
  This also adds a small security layer to determine who is able to run commands
  on these machines.
- Run the remote shell installer on the target machine, which will query for the
  url to your server, as well as your login information.
  This registers and launches the client and makes it visible in your dashboard.

The remote shell program sends a heartbeat to the server, which also serves as
a way to check its software version.
You can enable automatic software updates, in this case it is possible to sync
your version to the main one on demand and your machines will update to yours.
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

Server admins have full control and can view all keys by design, to ensure
transparency.
As data is not encrypted by default you are advised to use complementary
end-to-end encryption if safety is a concern.

  [1]: https://raw.githubusercontent.com/d-002/my_remote/refs/heads/main/shell_server/dashboard.png
