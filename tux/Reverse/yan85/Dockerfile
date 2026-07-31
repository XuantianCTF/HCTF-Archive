FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    openssh-server \
    gdb \
    vim \
    binutils \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/* \
    && pip3 install --no-cache-dir pwntools

RUN mkdir /var/run/sshd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin no/' /etc/ssh/sshd_config

RUN useradd -m -s /bin/bash ctf && \
    echo 'ctf:ctf' | chpasswd

COPY just_vm /home/ctf/just_vm

RUN echo "HCTF{Yan85_VM_Is_Fun}" > /flag

RUN chown root:root /flag && chmod 400 /flag
RUN chown root:root /home/ctf/just_vm && chmod 4755 /home/ctf/just_vm

EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]
