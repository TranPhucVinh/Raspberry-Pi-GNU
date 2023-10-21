FROM ubuntu:18.04

RUN apt-get update && apt-get install -y --no-install-recommends \
	bc \
	binutils \
	bison \
	bzip2 \
	ca-certificates \
	flex \
	gcc \
	g++ \
	gawk \
	git \
	gperf \
	libtool-bin \
	make \
	ncurses-dev \
	openssl \
	patch \
	texinfo \
	wget \
	sudo \
	vim \
	aptitude \
    gcc-arm-linux-gnueabihf

# Downgrade version for libssl-dev
RUN sudo aptitude install libssl-dev

RUN mkdir /raspberry
WORKDIR /raspberry
COPY util /raspberry/util

COPY linux /raspberry/linux

RUN touch /raspberry/linux/.config
RUN zcat /raspberry/util/build_material/config.gz > /raspberry/linux/.config

WORKDIR /raspberry/linux

RUN make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- oldconfig && make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)

RUN chmod 777 /raspberry/util/build.sh

ENTRYPOINT ["/raspberry/util/build.sh"]

