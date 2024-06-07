FROM mvadariripple/xrpl-plugin:latest as build

RUN python -c "import xrpl_plugin"

RUN apt-get update && apt-get install -y curl ca-certificates curl gnupg
# SHELL ["/bin/bash", "--login", "-c"]

ENV NODE_VERSION=20.14.0
ENV NVM_DIR /tmp/nvm
WORKDIR $NVM_DIR

RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash -
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y -q nodejs
RUN apt-get install -y aptitude
RUN aptitude install -y npm

RUN npm install --global xrpl

WORKDIR /answer
RUN npm link xrpl
COPY ./auction_create.py solution.py
RUN rm -rf /usr/local/lib/python3.10/dist-packages/xrpl_plugin/build_scripts/build
RUN plugin-build solution.py
RUN rm Solution.cpp
RUN echo "./rippled -a --conf rippled.cfg >debug.log & sleep 1 && node run.js; ./rippled -a --conf rippled.cfg stop" > run.sh
RUN chmod 777 run.sh

RUN ln -s /opt/build/rippled ./rippled

COPY ./rippled_answer.cfg ./rippled.cfg
COPY ./nftoken_escrow.js run.js

WORKDIR /apex
RUN npm link xrpl
COPY ./template.py ./nftoken_escrow.py
RUN plugin-build nftoken_escrow.py
RUN rm Auction.cpp

RUN ln -s /opt/build/rippled ./rippled

COPY ./rippled.cfg .
COPY ./template_run.js ./run.js
COPY ./nftoken_escrow.js /answer/run.js


RUN cp /answer/run.sh ./run.sh
RUN chmod 777 run.sh

RUN rm -rf ~/.conan/data



