# Dockerfile
FROM ubuntu:22.04

ARG SOURCE_FILE
ARG LAB_N
ARG COMPILE_CMD
ARG LANG

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    # mpich \
    # mpich-doc \
    openmpi-bin openmpi-common libopenmpi-dev  \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# RUN rm -rf /src

COPY /src/lab_${LAB_N} /app

COPY /src/lab_${LAB_N}/${SOURCE_FILE} /app/test.${LANG}

# RUN mpicc -o test test.c
# RUN mpicc -o test test.c -lm

RUN $COMPILE_CMD

RUN chmod +x /app/test

# CMD ["mpirun", "-np", "2", "./test"]
