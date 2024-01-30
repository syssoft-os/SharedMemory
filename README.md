# shared_memory.c

## Description
`shared_memory.c` is a program that demonstrates the usage of shared memory in inter-process communication. It creates a shared memory segment and allows to access and modify the shared data in subsequent calls.

### Commands
The program understands the following five commands as arguments:

1. `create <name> <size in bytes>`: Creates a new shared memory segment.
2. `list <name>`: Print a hexdump of the first 256 bytes of the shared memory segment.
3. `read <name> <offset>`: Reads the data from the shared memory segment at a given offset.
4. `write <name> <offset> <value>`: Writes data to the shared memory segment at a given offset.
5. `destroy <name>`: Deletes the shared memory segment.