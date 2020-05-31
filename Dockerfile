FROM ubuntu:18.04

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get autoremove

RUN apt-get install -y wget
RUN apt-get install -y netcat

WORKDIR /root

RUN wget -nv https://github.com/eosio/eos/releases/download/v2.0.5/eosio_2.0.5-1-ubuntu-18.04_amd64.deb
RUN apt-get install -y ./eosio_2.0.5-1-ubuntu-18.04_amd64.deb
RUN rm ./eosio_2.0.5-1-ubuntu-18.04_amd64.deb

COPY ./build/* ./build/
COPY ./scripts/run.sh ./scripts/run.sh
COPY ./scripts/init-chain.sh ./scripts/init-chain.sh
COPY ./scripts/seed.sh ./scripts/seed.sh
COPY ./scripts/stop.sh ./scripts/stop.sh
COPY ./default.env .env

# RUN ./scripts/run.sh
# RUN ./scripts/init-chain.sh
# RUN ./scripts/seed.sh

EXPOSE 8888

CMD [ "./scripts/run.sh" ]
#, "&&", "./scripts/init-chain.sh", "&&", "./scripts/seed.sh", "&&", "tail -f nodeos.log" ]

# ENTRYPOINT ["nodeos", \
#     "-e", \
#     "-p", \
#     "eosio", \
#     "--plugin eosio::chain_api_plugin", \
#     "--plugin eosio::http_plugin", \
#     "--plugin eosio::history_plugin", \
#     "--plugin eosio::history_api_plugin", \
#     "--filter-on='*'", \
#     "--access-control-allow-origin='*'", \
#     "--contracts-console", \
#     "--http-validate-host=false", \
#     "--replay-blockchain", \
#     "--hard-replay-blockchain", \
#     "--verbose-http-errors >> nodeos.log 2>&1 &"]
# # "--plugin eosio::producer_api_plugin", \
# # "--plugin eosio::producer_plugin", \
