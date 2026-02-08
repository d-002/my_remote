## user
+ create user, log in and out, with username and password
+ queue new commands to a machine
+ list command history
+ list linked machines
+ read the state of the connected machines (lower timeout when currently writing
  commands)
+ change machines names
+ clear command history
+ pages styling

- handle and display status messages
- view global files version and sync update, toggle to disable updates
- send custom messages to a machine: restart, destroy (as special keywords in a
  command)
- get machine state easily using an api call
- update machines data etc periodically

## machine
+ register and attach to user
+ read commands
+ remove user commands from the queue (mark them as read)
+ send back commands
+ send heartbeat with version, get back success, error or the new version and
  binary
+ automatically update on new version
+ installer: ask for url, login, fetch the hashes and copy them to files, fetch
  the binary code from the server and run it.
- set state (heartbeat every 10s in idle mode, when receiving a message
  heartbeat every second for 30s and set state to ready, then back to idle if no
  further messages within these 30s)

- send status messages: command timeout, errors related to the shell, feedback
  for restart, destroy
- use poll() and timeouts for reliability
- execute simple commands, handle and listen for api calls
- execute commands using a shell and pipe the streams
- handle custom commands like restart and destroy
