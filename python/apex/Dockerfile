FROM mvadariripple/xrpl-plugin:latest as build

RUN pip install xrpl-plugin --upgrade -v
RUN python -c "import xrpl_plugin"

COPY ./nftoken_escrow.py /answer/solution.py
COPY ./template.py /work/nftoken_escrow.py

WORKDIR /work
RUN plugin-build nftoken_escrow.py

COPY ./rippled.cfg .
