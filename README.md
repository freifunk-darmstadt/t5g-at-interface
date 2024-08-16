# t5g-at-interface

Quick and dirty programm for interfacing with the Modem AT Command interface on the host system of a Telekom 5G Empfänger SE.

## TL;DR

```bash
# Download from GitHub
wget -O /data/t5g-at https://github.com/freifunk-darmstadt/t5g-at-interface/releases/download/v0.0.1/t5g-at
# Make Executable
chmod +x /data/t5g-at
# Run
/data/t5g-at
```

## How to compile

```bash
wget -O /tmp/toolchain.tar.xz https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz
tar -xf /tmp/toolchain.tar.xz -C /tmp
make CROSS_COMPILE=/tmp/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-
```

## How to use

```bash
./t5g-at-interface
```
