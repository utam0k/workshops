#/bin/bash

echo "deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_20.04/ /" | tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list
curl -L "https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_20.04/Release.key" | apt-key add -
apt update
apt -y upgrade
apt install -y podman bpftrace
mount -t debugfs none /sys/kernel/debug

apt-get install -y \
  bison \
  cmake \
  flex \
  g++ \
  git \
  libelf-dev \
  zlib1g-dev \
  libfl-dev \
  systemtap-sdt-dev \
  binutils-dev \
  libcereal-dev \
  llvm-dev \
  llvm-runtime \
  libclang-dev \
  clang \
  libpcap-dev \
  libgtest-dev \
  libgmock-dev \
  asciidoctor \
  xxd \
  pahole
git clone https://github.com/iovisor/bpftrace --recurse-submodules /tmp/bpftrace/
mkdir /tmp/bpftrace/build; cd /tmp/bpftrace/build;
cd /tmp/bpftrace/build; ../build-libs.sh;
cd /tmp/bpftrace/build; cmake -DCMAKE_BUILD_TYPE=Release ..;
cd /tmp/bpftrace/build; make -j8;
cd /tmp/bpftrace/build; make install;
