FROM ubuntu@sha256:0e5e4a57c2499249aafc3b40fcd541e9a456aab7296681a3994d631587203f97

RUN apt update -y && apt upgrade -y

RUN useradd -m ctf
RUN echo "ctf:ctf" | chpasswd

WORKDIR /home/ctf
COPY ynetd .
COPY chal .

RUN chmod +x /home/ctf/chal
RUN chmod +x /home/ctf/ynetd

USER ctf
EXPOSE 1337
CMD echo $FLAG > flag.txt && ./ynetd -p 1337 "timeout 60 ./chal"