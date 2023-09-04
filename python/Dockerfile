FROM ubuntu:latest as build

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y git cmake software-properties-common \
    pkg-config zlib1g \
    libssl-dev
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get install -y gcc-11 g++-11 vim nano gdb && \
    update-alternatives --remove-all cpp && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 \
    --slave /usr/bin/gcov gcov /usr/bin/gcov-11 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-11 \
    --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-11  --slave /usr/bin/cpp cpp /usr/bin/cpp-11

WORKDIR /opt
RUN git clone --depth 1 --branch plugin https://github.com/mvadari/rippled.git

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y python3 python3-dev
RUN apt-get install -y python3-pip python-is-python3
RUN pip install conan==1.60.2

WORKDIR /opt

ENV LD_LIBRARY_PATH=/usr/local/lib/python3.10/dist-packages/xrpl_plugin

RUN mkdir build && cd build
RUN cd /opt/build && conan install ../rippled --build missing --settings build_type=Debug
RUN cd /opt/build && cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug /opt/rippled -Dtests=Off

RUN cd /opt/build && cmake --build . --parallel $(nproc)

WORKDIR /work
COPY ./rippled.cfg .

RUN ln -s /opt/build/rippled ./rippled

# COPY ./dist/xrpl-plugin-0.0.4.tar.gz .
RUN pip install xrpl-plugin -v
RUN python -c "import xrpl_plugin"

COPY ./examples .
RUN python dummy_tx.py
RUN plugin-build dummy_tx.py
RUN plugin-build new_escrow_create.py
