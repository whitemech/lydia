FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL C.UTF-8
ENV LANG C.UTF-8

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
       clang \
       gcc \
       g++ \
       g++-multilib \
       gdb \
       clang-tidy \
       clang-format \
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


RUN sudo apt-get install -y flex bison libgraphviz-dev libboost-all-dev

WORKDIR /home/default

RUN wget https://github.com/whitemech/cudd/releases/download/v3.0.0/cudd_3.0.0_linux-amd64.tar.gz &&\
    tar -xf cudd_3.0.0_linux-amd64.tar.gz &&\
    cd cudd_3.0.0_linux-amd64 &&\
    sudo cp -P lib/* /usr/local/lib/ &&\
    sudo cp -Pr include/cudd/* /usr/local/include

RUN wget https://github.com/whitemech/MONA/releases/download/v1.4-18.dev0/mona_1.4-18.dev0_linux-amd64.tar.gz &&\
    tar -xf mona_1.4-18.dev0_linux-amd64.tar.gz &&\
    cd mona_1.4-18.dev0_linux-amd64 &&\
    sudo cp -P lib/* /usr/local/lib/ &&\
    sudo cp -Pr include/* /usr/local/include

RUN git clone https://github.com/whitemech/Syft.git &&\
    cd Syft &&\
    git checkout syft+ &&\
    mkdir build && cd build &&\
    cmake -DCMAKE_BUILD_TYPE=Release .. &&\
    make -j &&\
    sudo make install

#RUN wget http://ftp.us.debian.org/debian/pool/main/b/bison/libbison-dev_3.0.4.dfsg-1+b1_amd64.deb &&\
#  wget http://ftp.us.debian.org/debian/pool/main/b/bison/bison_3.0.4.dfsg-1+b1_amd64.deb &&\
#  sudo dpkg -i libbison-dev_3.0.4.dfsg-1+b1_amd64.deb &&\
#  sudo dpkg -i bison_3.0.4.dfsg-1+b1_amd64.deb


USER default

WORKDIR /build

ENTRYPOINT []
