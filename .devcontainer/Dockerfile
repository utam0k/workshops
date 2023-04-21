FROM ubuntu:22.04

RUN apt update
RUN apt -y upgrade
RUN apt install -y curl gnupg
RUN echo "deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_20.04/ /" | tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list
RUN curl -L "https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_20.04/Release.key" | apt-key add -
RUN apt install -y podman sudo

RUN apt-get install -y \
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
RUN git clone https://github.com/iovisor/bpftrace --recurse-submodules /tmp/bpftrace/
RUN mkdir /tmp/bpftrace/build; cd /tmp/bpftrace/build;
RUN cd /tmp/bpftrace/build; ../build-libs.sh;
RUN cd /tmp/bpftrace/build; cmake -DCMAKE_BUILD_TYPE=Release ..;
RUN cd /tmp/bpftrace/build; make -j8;
RUN cd /tmp/bpftrace/build; make install;

ARG USERNAME=ubuntu
ARG GROUPNAME=ubuntu
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID $GROUPNAME && \
    useradd -m -s /bin/bash -u $UID -g $GID $USERNAME
RUN echo '$USERNAME ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER $USERNAME
WORKDIR /home/$USERNAME/
