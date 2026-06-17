FROM ubuntu:22.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    openssh-server \
    gcc \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /var/run/sshd && \
    ssh-keygen -A

RUN useradd -m -s /bin/bash ctf && \
    echo 'ctf:ctf' | chpasswd

RUN mkdir -p /home/ctf/.ssh && \
    chmod 700 /home/ctf/.ssh && \
    touch /home/ctf/.ssh/authorized_keys && \
    chmod 600 /home/ctf/.ssh/authorized_keys && \
    chown -R ctf:ctf /home/ctf/.ssh

RUN echo '只要你能运行这个程序，就能拿到 flag' > /home/ctf/hint.txt && \
    chmod 644 /home/ctf/hint.txt && \
    chown ctf:ctf /home/ctf/hint.txt

RUN echo '你或许会C语言，但是C语言和内核之间是如何交接的呢？' > /home/ctf/hint.txt

RUN printf 'a0804f0aa29e95bcf04dc25501aab2ed2e95bf044279ea\n' > /flag && \
    chmod 444 /flag && \
    chown root:root /flag

COPY chall.c /chall.c

RUN gcc -o /home/ctf/chall /chall.c && \
    chmod 644 /home/ctf/chall && \
    chown root:root /home/ctf/chall && \
    rm -f /chall.c

RUN rm -f /usr/bin/chmod /bin/chmod

EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]
