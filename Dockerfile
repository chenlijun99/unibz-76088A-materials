FROM ubuntu:focal
LABEL maintainer="Lijun Chen"

# See https://github.com/moby/moby/issues/4032
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
	apt-get install -y \
	# See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-setup.html#install-prerequisites
	# Mind the SDK version!
	git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 \
	build-essential

RUN mkdir -p ~/esp && \
cd ~/esp && \
git clone --recursive --branch v4.4 --single-branch https://github.com/espressif/esp-idf.git && \
cd esp-idf/ && \
./install.sh

COPY . /unibz-76088A/
WORKDIR /unibz-76088A/
