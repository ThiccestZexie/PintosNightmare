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
    gdb \
    curl \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Add utils to path
ENV PATH="$PATH:/pintos/utils"

RUN curl -sS https://starship.rs/install.sh | sh -s -- --yes \
	&& echo 'eval "$(starship init bash)"' >> ~/.bashrc
RUN mkdir -p ~/.config && cat <<EOF > ~/.config/starship.toml
add_newline=false
[directory]
truncate_to_repo = true
[character]
error_symbol = "[✗](bold red)"
[container]
disabled = true
[username]
disabled = true
EOF


WORKDIR /pintos

CMD ["bash"]
