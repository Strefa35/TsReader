# Build environment for TsReader
# Ubuntu 24.04 + wxWidgets 3.3.2 (built from source)

FROM ubuntu:24.04

ARG WXWIDGETS_VERSION=3.3.2
ARG DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    # Build tools
    cmake \
    make \
    g++ \
    pkg-config \
    git \
    ca-certificates \
    curl \
    bzip2 \
    # wxWidgets build dependencies
    libgtk-3-dev \
    libglib2.0-dev \
    libpango1.0-dev \
    libcairo2-dev \
    libgdk-pixbuf-2.0-dev \
    libatk1.0-dev \
    libx11-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    zlib1g-dev \
    libexpat1-dev \
    libpcre2-dev \
    && rm -rf /var/lib/apt/lists/*

# Download and build wxWidgets 3.3.2 from source
RUN curl -L "https://github.com/wxWidgets/wxWidgets/releases/download/v${WXWIDGETS_VERSION}/wxWidgets-${WXWIDGETS_VERSION}.tar.bz2" \
        -o /tmp/wxwidgets.tar.bz2 \
    && mkdir -p /tmp/wxwidgets \
    && tar -xjf /tmp/wxwidgets.tar.bz2 -C /tmp/wxwidgets --strip-components=1 \
    && mkdir -p /tmp/wxwidgets-build \
    && cd /tmp/wxwidgets-build \
    && /tmp/wxwidgets/configure \
        --prefix=/usr/local \
        --disable-shared \
        --with-gtk=3 \
        --without-libtiff \
        --disable-richtext \
    && make -j"$(nproc)" \
    && make install \
    && rm -rf /tmp/wxwidgets /tmp/wxwidgets-build /tmp/wxwidgets.tar.bz2

WORKDIR /workspace
