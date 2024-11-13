FROM ubuntu:24.04

RUN apt-get -y update && \
    apt-get -y upgrade

RUN useradd -m ctf
RUN echo "ctf:ctf" | chpasswd
WORKDIR /home/ctf

COPY rudra .
COPY libs/ libs/
COPY ynetd .

COPY ./libs/libstdc++.so.6 /lib/x86_64-linux-gnu/libstdc++.so.6 
COPY ./libs/libgcc_s.so.1 /lib/x86_64-linux-gnu/libgcc_s.so.1 
COPY ./libs/libc.so.6 /lib/x86_64-linux-gnu/libc.so.6 
COPY ./libs/libm.so.6 /lib/x86_64-linux-gnu/libm.so.6 
COPY ./libs/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2 

USER ctf
EXPOSE 1337
CMD echo $FLAG > flag.txt &&./ynetd -p 1337 ./rudra