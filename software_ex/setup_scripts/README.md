# About
This is my attempt at automating tool collection of my frequently used tools. It works well on Ubuntu 20.04 forward from my limited testing. Breaks pretty bad on Ubuntu 18.04.

This script is highly opinionated and attempts to insert my prefered configurations for these tools. Please make sure to adjust the script and configs as you deem fit.

# Operation
To run the installation, simply run `bash get_ready.sh`.
Change these things:
- `USER=set_ur_username`
- Change repositories to closest mirror preference

# Notes
This installs my preferred tools ranging from simple build utilities to my prefered hex editor. Please modify this list as you desire.

Additionally, depending on the apt version of GDB, you may wish to run the `gdb_update.sh` to pull directly from the GDB sources and get an improved version. GEF and PWNDBG have python requirements greater than 3.9 I believe, so consider this as well when building GDB.

Lastly, while this script can reduce some work with trying to initially setup a system, it is not entirely capable of total autonomous operation yet and needs some manual help to finalize a few details.

# Future works
Polish this to work consistently on stable Ubuntu releases.
Will remove the file dependencies and create files entirely from script in future. These were testing additions and have yet to be fully enrolled to the script.
