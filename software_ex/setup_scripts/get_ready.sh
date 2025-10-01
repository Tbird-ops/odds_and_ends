#!/bin/bash
## Setup Ubuntu linux system for binary analysis and exploitation
## Note: This script is very opinionated

# Repo tools/configs directory
TOOLS_DIR=$PWD
USER=ubuntu
USER_HOME=/home/$USER

[[ $EUID -ne 0 ]] && echo "Need root" && exit 1

# Building update script
echo Making /update
cat > /update << EOL
#!/bin/bash
[[ \$EUID -ne 0 ]] && echo "Need root" && exit 1
apt clean
apt update -y
apt upgrade -y
apt autoremove -y
[ -f /var/run/reboo* ] && echo -e '\n\nNeed to reboot!\n'
EOL
chown root:root /update
chmod 555 /update


echo configuring apt repositories
cp /etc/apt/sources.list /etc/apt/sources.list.backup
if lsb_release -a 2>/dev/null | grep -iq ubuntu; then
	echo Ialab repos configured
	cat > /etc/apt/sources.list << EOL
deb http://repo.ialab.dsu.edu/ubuntu $(lsb_release -sc) main restricted universe multiverse
deb http://repo.ialab.dsu.edu/ubuntu $(lsb_release -sc)-updates main restricted universe multiverse
deb http://repo.ialab.dsu.edu/ubuntu $(lsb_release -sc)-security main restricted universe multiverse
deb http://repo.ialab.dsu.edu/ubuntu $(lsb_release -sc)-backports main restricted universe multiverse
EOL
else
	echo Repos not configured. This is not ubuntu
fi

# System repository update
apt update
# Extra tools and repositories will be cloned to ** /opt **

# Helpful system things
if [[ $XDG_SESSION_TYPE -eq 'x11' ]]; then
	apt install bash-completion flatpak htop btop nano screen vim tmux git xclip -y
else
	apt install bash-completion flatpak htop btop nano screen vim tmux git wl-clipboard -y
fi

# Configure flatpak
flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
# Configure FZF
cd /opt && git clone https://github.com/junegunn/fzf.git && chown -R $USER:$USER fzf
sudo -i -u $USER bash << EOF
cd /opt/fzf/
yes | ./install
EOF

# Get bashmarks setup
cd /opt && git clone https://github.com/huyng/bashmarks.git
chmod -R 755 /opt/bashmarks
sudo -i -u $USER bash << EOF
cd /opt/bashmarks/ && make install
cd $USER_HOME/
mv .bashrc .bashrc.bak
cp $TOOLS_DIR/bashrc .bashrc
cp $TOOLS_DIR/bash-aliases .bash_aliases
mkdir -p $USER_HOME/.local/bin/
EOF

# Updating system bashrc
echo Updating bashrc
cat >> /etc/bash.bashrc << EOL
# Colors
if [ \$(id -u) -eq 0 ]; then # you are root, make the prompt red
  PS1='\${debian_chroot:+($debian_chroot)}\[\033[01;31m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
else
  PS1='\${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
fi

# enable bash completion in interactive shells
if ! shopt -oq posix; then
  if [ -f /usr/share/bash-completion/bash_completion ]; then
    . /usr/share/bash-completion/bash_completion
  elif [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
  fi
fi
alias ll='ls -laFtr'
alias la='ls -al'
alias ls='ls --color=auto'
EOL

# Special configurations
echo setup nano
cat >> /etc/nanorc << EOL
bind ^S savefile main
set positionlog
EOL

echo setup tmux
cat >> $USER_HOME/.tmux.conf << EOL
# Make mouse on 'm'
bind m \
        set -g mouse on \;\
        display 'Mouse: ON'
# Make mouse off 'M'
bind M \
        set -g mouse off \;\
        display 'Mouse: OFF'
EOL

echo setup screen
cat >> /etc/screenrc << EOL
termcapinfo xterm* ti@:te@
EOL

echo disable bracked paste
cat >> /etc/inputrc << EOL
set enable-bracketed-paste 0
EOL

# Helpful network things
apt install curl wget net-tools netcat-openbsd nmap dnsutils traceroute tcpdump -y

# Helpful software dev
apt install build-essential gcc gcc-multilib clang clang-tools xxd binutils cppcheck clang clang-tools ninja-build libssl-dev libffi-dev -y

# Get Docker
## Move to root
cd /root
## Get deps
apt install apt-transport-https ca-certificates software-properties-common -y
## Pull keys
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
## Add repo 
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
## Pull updated info
apt update
## Repo policy enforcement
apt-cache policy docker-ce
## Install
apt install docker-ce -y
## Add user to docker group
usermod -aG docker $USER

# Python things
# install python and remove external system management (personal preference thing, I realize its not best practice)
apt install python3-dev python3-pip python3-venv libbz2-dev libreadline-dev libsqlite3-dev -y && find /lib/ -name EXTERNALLY-MANAGED -exec mv {} {}.backup \;
# Get pwntools configured
pip install --upgrade pip && pip install capstone filebytes keystone-engine pyvex ropper pwntools
# Pyenv Version tracker
sudo -i -u $USER bash << EOF
curl -fsSL https://pyenv.run | bash
EOF

# Rust things
# Rust-lang
sudo -i -u $USER bash << EOF
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
EOF
# Weggli
sudo -i -u $USER bash << EOF
cargo install weggli
EOF

# GDB things
# Get GDB
apt install gdb gdbserver gdb-multiarch -y
# Get GDB GEF and PWNDBG
sudo -i -u $USER bash << EOF
cd $USER_HOME/
bash -c "$(curl -fsSL https://gef.blah.cat/sh)"
curl -qsL 'https://install.pwndbg.re' | sh -s -- -t pwndbg-gdb
mv .gdbinit .gdbinit.bak
cp $TOOLS_DIR/gdbinit .gdbinit
cp $TOOLS_DIR/gdb-pwn $USER_HOME/.local/bin/gdb-pwn
cp $TOOLS_DIR/gdb-gef $USER_HOME/.local/bin/gdb-gef
echo "DON'T FORGET TO SETUP BOTH INSTANCES WITH GDBINIT"
EOF

# Get ImHex
sudo -i -u $USER bash << EOF
flatpak install flathub net.werwolv.ImHex
EOF

# Get Radare2
cd /opt && git clone https://github.com/radareorg/radare2 && chown -R $USER:$USER radare2
sudo -i -u $USER bash << EOF
cd /opt/radare2; sys/install.sh
EOF

# Fuzzing tools
# If AFL++ is not in repo list, check github
# LibFuzzer should be available from the clang clang-tools install above
cd /opt && git clone https://github.com/AFLplusplus/AFLplusplus && chown -R $USER:$USER AFLplusplus
cd /opt/AFLplusplus
make distrib
make install

# Clone LibAFL for building custom fuzzers
cd /opt && git clone https://github.com/AFLplusplus/LibAFL.git && chown -R $USER:USER LibAFL

# Preeny
cd /opt && git clone https://github.com/zardus/preeny

# Look into CodeQL to do querying across larger codebases
# Look into semgrep for doing similar large code base searches

screen -mdS update /update
echo Updated started in screen
