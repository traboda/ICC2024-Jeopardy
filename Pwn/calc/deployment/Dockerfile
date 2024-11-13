FROM ubuntu:24.04@sha256:99c35190e22d294cdace2783ac55effc69d32896daaa265f0bbedbcde4fbe3e5

RUN apt update -y && apt upgrade -y
RUN apt install -y g++

RUN useradd -m ctf
RUN echo "ctf:ctf" | chpasswd

WORKDIR /home/ctf
COPY ynetd .
COPY calc .

RUN chmod +x /home/ctf/calc
RUN chmod +x /home/ctf/ynetd

USER ctf
EXPOSE 1337
CMD echo $FLAG > flag.txt && ./ynetd -p 1337 "timeout 60 ./calc"