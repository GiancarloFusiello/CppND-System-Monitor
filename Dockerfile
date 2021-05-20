FROM ubuntu:20.04 AS dev

ARG DEV_DEPENDENCIES="git net-tools psmisc htop"
ARG BUILD_DEPENDENCIES="build-essential cmake gdb"
ARG THIRD_PARTY_DEPENDENCIES="libncurses5-dev libncursesw5-dev"

ENV DEBIAN_FRONTEND=noninteractive

RUN set -ex \
    && apt update -y \
    && apt install -y --no-install-recommends \
        ${DEV_DEPENDENCIES} \
        ${BUILD_DEPENDENCIES} \
        ${THIRD_PARTY_DEPENDENCIES} \
    && apt autoremove \
    && apt clean \
    && rm -rf ~/.cache/* /var/lib/apt/lists/* /var/cache/apt/* /tmp/*


FROM dev AS launch

WORKDIR /srv/app/build

COPY . ..

RUN cmake .. && make

CMD monitor
