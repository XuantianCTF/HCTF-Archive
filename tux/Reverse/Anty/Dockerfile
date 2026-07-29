FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    openssh-server \
    gcc \
    gdb \
    vim \
    binutils \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /var/run/sshd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin no/' /etc/ssh/sshd_config

RUN useradd -m -s /bin/bash ctf && \
    echo 'ctf:ctf' | chpasswd

COPY main.c check.S /tmp/

RUN gcc -no-pie /tmp/main.c /tmp/check.S -o /home/ctf/challenge

RUN echo "$(cat flag)" > /flag

RUN chown root:root /flag && chmod 400 /flag
RUN chown root:root /home/ctf/challenge && chmod 4755 /home/ctf/challenge
RUN rm /tmp/main.c /tmp/check.S

EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]
