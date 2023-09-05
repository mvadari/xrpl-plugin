FROM mvadariripple/xrpl-plugin:latest as build

RUN pip install xrpl-plugin --upgrade -v
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

WORKDIR /apex
RUN npm link xrpl
COPY ./template.py ./nftoken_escrow.py
RUN plugin-build nftoken_escrow.py
RUN rm NewEscrowCreate.cpp

COPY ./nftoken_escrow.py /answer/solution.py
RUN ln -s /opt/build/rippled ./rippled

COPY ./rippled.cfg .
COPY ./nftoken_escrow.js ./run.js

RUN echo "./rippled -a --conf rippled.cfg >debug.log & sleep 1 && node run.js; pgrep rippled | xargs kill -9" > run.sh
RUN chmod 777 run.sh
