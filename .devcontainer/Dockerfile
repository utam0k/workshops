FROM ubuntu:latest
RUN apt update
RUN apt install -y sudo build-essential linux-generic libmpich-dev libopenmpi-dev libblas-dev liblapack-dev libatlas-base-dev time python3-pip

RUN useradd --uid 1000 --create-home --shell /bin/bash -G sudo,root ubuntu
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER ubuntu
RUN pip install numpy