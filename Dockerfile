# Dockerfile
FROM ubuntu:22.04

ARG SOURCE_FILE

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    mpich \
    mpich-doc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# COPY /src/broadcast.c /app/test.c
COPY /src/${LAB_B}/${SOURCE_FILE} /app/test.c

RUN mpicc -o test test.c

RUN chmod +x /app/test

# CMD ["mpirun", "-np", "2", "./test"]
