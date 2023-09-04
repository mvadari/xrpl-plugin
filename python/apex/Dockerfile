FROM mvadariripple/xrpl-plugin:latest as build

RUN pip install xrpl-plugin -v
RUN python -c "import xrpl_plugin"

COPY ./nftoken_escrow.py /answer