FROM ubuntu:bionic

# Set environment variables
#ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Stockholm

# Update package lists and install required packages
RUN apt-get update -qq && \
    apt-get install -y --no-install-recommends \
    build-essential \
    qemu \
    qemu-system-x86 \
    xauth \
    bash \
    gdbserver \
    gdb && \
    rm -rf /var/lib/apt/lists/*

# Add utils to path
ENV PATH="$PATH:/pintos/utils"

WORKDIR /pintos

CMD ["bash"]
