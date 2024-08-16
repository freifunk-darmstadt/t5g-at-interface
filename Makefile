CROSS_COMPILE ?= 
GCC ?= $(CROSS_COMPILE)gcc
STRIP ?= $(CROSS_COMPILE)strip

# Take git short-hash as version
VERSION ?= $(shell git rev-parse --short HEAD)

# Flags
CFLAGS = --static -O2 -DVERSION=\"$(VERSION)\"

all:
	mkdir -p output
	$(GCC) $(CFLAGS) -o output/t5g-at t5g.c
	$(STRIP) output/t5g-at