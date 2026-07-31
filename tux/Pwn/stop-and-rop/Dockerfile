FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends socat && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -m -u 1000 challenge

WORKDIR /challenge

COPY stop-pop-and-rop-hard /challenge/stop-pop-and-rop-hard

COPY flag /flag

RUN chmod 755 /challenge/stop-pop-and-rop-hard

USER challenge

EXPOSE 1337

CMD ["socat", "TCP-LISTEN:1337,reuseaddr,fork", "EXEC:/challenge/stop-pop-and-rop-hard,pty,stderr,setsid,sigint,sane,raw"]
