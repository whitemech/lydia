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


USER default

RUN sudo apt-get install -y flex libgraphviz-dev

WORKDIR /home/default

RUN git clone https://github.com/KavrakiLab/cudd --recursive && \
  cd cudd && \
  autoreconf -i && \
  ./configure --enable-silent-rules --enable-obj --enable-dddmp && \
  sudo make install

RUN wget http://ftp.us.debian.org/debian/pool/main/b/bison/libbison-dev_3.0.4.dfsg-1+b1_amd64.deb &&\
  wget http://ftp.us.debian.org/debian/pool/main/b/bison/bison_3.0.4.dfsg-1+b1_amd64.deb &&\
  sudo dpkg -i libbison-dev_3.0.4.dfsg-1+b1_amd64.deb &&\
  sudo dpkg -i bison_3.0.4.dfsg-1+b1_amd64.deb

WORKDIR /build

ENTRYPOINT []
