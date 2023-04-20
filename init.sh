#/bin/bash

echo "deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_20.04/ /" | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list
curl -L "https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_20.04/Release.key" | sudo apt-key add -
sudo apt update
sudo apt -y upgrade
sudo apt install -y podman bpftrace dkms linux-headers-$(uname -r)
sudo mount -t debugfs none /sys/kernel/debug
sudo modprobe kheaders

git clone https://github.com/brauner/mount-idmapped.git
cd mount-idmapped
gcc mount-idmapped.c -o idmapped
sudo mv idmapped /usr/bin/
cd ../

git clone https://github.com/toby63/shiftfs-dkms.git
cd shiftfs-dkms
git checkout k5.4
./update1
