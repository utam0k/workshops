FROM ubuntu:latest
RUN apt update
RUN apt install -y build-essential linux-generic libmpich-dev libopenmpi-dev libblas-dev liblapack-dev libatlas-base-dev 
RUN pip3 install numpy
