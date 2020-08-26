FROM ubuntu:18.04 as build

RUN apt-get update && \
    apt-get install -y \
      binutils-mips-linux-gnu \
      bsdmainutils \
      build-essential \
      imagemagick \
      libaudiofile-dev \
      pkg-config \
      python3 \
      wget \
      zlib1g-dev

RUN wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb \
  -O devkitpro.deb && \
  echo ebc9f199da9a685e5264c87578efe29309d5d90f44f99f3dad9dcd96323fece3 devkitpro.deb | sha256sum --check && \
  apt install -y ./devkitpro.deb && \
  rm devkitpro.deb
RUN dkp-pacman -Syu 3ds-dev --noconfirm

RUN mkdir /sm64
WORKDIR /sm64

ENV PATH="/opt/devkitpro/tools/bin/:/sm64/tools:${PATH}"
ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=/opt/devkitpro/devkitARM
ENV DEVKITPPC=/opt/devkitpro/devkitPPC

CMD echo 'usage: docker run --rm --mount type=bind,source="$(pwd)",destination=/sm64 sm64 make VERSION=${VERSION:-us} -j4\n' \
         'see https://github.com/n64decomp/sm64/blob/master/README.md for advanced usage'
