## user
+ create user, log in and out, with username and password
+ queue new commands to a machine
+ list command history
+ list linked machines

- change machines names
- read the state of the connected machines (lower timeout when currently writing
  commands)
- send custom messages to a machine: restart, destroy (as special keywords in a
  command)
- delete account and everything related to it

## machine
+ register and attach to user
+ read commands

- remove user commands from the queue (mark them as read)
- send back commands
- send heartbeat with state, version, get back success, error or the new version
  and binary
- execute commands using a shell and pipe the streams
- handle custom commands like restart and destroy
- set state (heartbeat every 10s in idle mode, when receiving a message
  heartbeat every second for 30s and set state to ready, then back to idle if no
  further messages within these 30s)
