wsl sudo apt-get update;sudo apt install g++ gdb make rsync zip unzip python3 python3-pip; python3 -m pip install scons debugpy;sudo ssh-keygen -A;sudo cp ./wsl/sshd_config /etc/ssh/sshd_config; sudo systemctl enable ssh; sudo service ssh start;
pause