#!/bin/bash

# Fix an issue with pwntools on Ubuntu 20.04 + GDB 9.2
#
# GDBServer doesn't work right with 32-bit programs, which
# causes gdb.debug() to crash in __ctype_init().
#
# This script solves the problem by compiling a newer version
# of GDB from source, replacing the default versions which
# were installed from the repo.
#
# Ref:
# - https://github.com/Gallopsled/pwntools/issues/1783
# - https://secure77.de/pwntools-gdb-gdbserver-and-pwndbg-with-32-bit-binaries/

# Change this if you want a newer version of GDB
VERSION=16.2

# Make sure we're running as root, so we can install things
if [ $UID != 0 ]
then
		echo "ERROR: Must be run as root"
			exit
fi

# Remove current GDB and install dependencies to compile the new one
apt update
apt install -y libgmp-dev libreadline-dev texinfo libmpfr-dev
apt remove -y gdb gdbserver

# Obtain the source code
cd /usr/local/src/
wget https://ftp.gnu.org/gnu/gdb/gdb-$VERSION.tar.xz
tar xvf gdb-$VERSION.tar.xz
cd gdb-$VERSION/

# Build it
mkdir build
cd build
../configure --prefix=/usr/local \
		--with-system-readline \
		--with-python=/usr/bin/python3 \
		--disable-intl 
make -j8

# Install it
make -C gdb install
make -C gdbserver install
