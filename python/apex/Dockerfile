FROM mvadariripple/xrpl-plugin:latest as build

RUN python -c "import xrpl_plugin"

RUN apt-get install -y --no-install-recommends apt-utils && apt-get install curl -y

RUN apt-get install -y ca-certificates curl gnupg
# SHELL ["/bin/bash", "--login", "-c"]

ENV NODE_VERSION=18.12.1
ENV NVM_DIR /tmp/nvm
WORKDIR $NVM_DIR

RUN curl -fsSL https://deb.nodesource.com/setup_18.x | bash -
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y -q nodejs
RUN apt-get install -y aptitude
RUN aptitude install -y npm

RUN npm install --global xrpl

WORKDIR /answer
RUN npm link xrpl
COPY ./nftoken_escrow.py solution.py
RUN plugin-build solution.py
RUN rm NewEscrowCreate.cpp
RUN echo "./rippled -a --conf rippled.cfg >debug.log & sleep 1 && node run.js; ./rippled -a --conf rippled.cfg stop" > run.sh
RUN chmod 777 run.sh

RUN ln -s /opt/build/rippled ./rippled

COPY ./rippled_answer.cfg ./rippled.cfg
COPY ./nftoken_escrow.js run.js

WORKDIR /apex
RUN npm link xrpl
COPY ./template.py ./nftoken_escrow.py
RUN plugin-build nftoken_escrow.py
RUN rm NewEscrowCreate.cpp

RUN ln -s /opt/build/rippled ./rippled

COPY ./rippled.cfg .
COPY ./template_run.js ./run.js
COPY ./nftoken_escrow.js /answer/run.js


RUN cp /answer/run.sh ./run.sh
RUN chmod 777 run.sh

RUN rm -rf ~/.conan/data



