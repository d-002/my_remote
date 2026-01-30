# my_remote
hopefully reliable interface for a remote shell without any ip nonsense

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

> [!NOTE]  
> Note: this should support multiple clients, but logging as of now will be
> unpredictable as responses are automatically deleted upon reading.
