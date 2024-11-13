FROM ubuntu@sha256:99c35190e22d294cdace2783ac55effc69d32896daaa265f0bbedbcde4fbe3e5

RUN export TERM="xterm-256color"

RUN apt-get -y update && \
    apt-get -y upgrade

RUN apt-get install -y \
    libssl-dev

RUN useradd -m ctf
RUN echo "ctf:ctf" | chpasswd
WORKDIR /home/ctf

COPY lockboxx .
COPY ynetd .

RUN chmod +x lockboxx
RUN chmod +x ynetd

USER ctf
EXPOSE 1337
CMD echo $FLAG > flag.txt && ./ynetd -p 1337 ./lockboxx
