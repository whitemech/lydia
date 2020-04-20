FROM ubuntu:19.10

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
       clang \
       gcc \
       g++ \
       g++-multilib \
       gdb \
       clang-tidy \
       clang-format-9 \
       gcovr \
       llvm \
       sudo \
       vim \
       make \
       cmake \
       automake \
       libtool \
       git \
       less \
       curl \
       wget


# This adds the 'default' user to sudoers with full privileges:
RUN HOME=/home/default && \
    mkdir -p ${HOME} && \
    GROUP_ID=1000 && \
    USER_ID=1000 && \
    groupadd -r default -f -g "$GROUP_ID" && \
    useradd -u "$USER_ID" -r -g default -d "$HOME" -s /sbin/nologin \
    -c "Default Application User" default && \
    chown -R "$USER_ID:$GROUP_ID" ${HOME} && \
    usermod -a -G sudo default && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++
ENV CCACHE_DIR=/build/docker_ccache


USER default

RUN sudo apt-get install -y flex bison libgraphviz-dev

WORKDIR /home/default

RUN git clone https://github.com/KavrakiLab/cudd --recursive && \
cd cudd && \
autoreconf -i && \
./configure --enable-silent-rules --enable-obj --enable-dddmp && \
sudo make install

WORKDIR /build

ENTRYPOINT []
