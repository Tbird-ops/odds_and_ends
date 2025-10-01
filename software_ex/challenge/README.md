# SYSTEM.SECURE

*A little uninitialized variable PWN challenge I designed for a final project in a software exploitation class. Can you figure it out?*
Note! This was tested and worked on Ubuntu 20.04. There may be difficulties with this in other environments due to differences in compilers or Libc versions.

## BUILDING:

In theory, its as easy as
$ make

If that doesn't work, try:
gcc -no-pie -o final.bin system.secure.c

To clean up directory, run
$ make clean
