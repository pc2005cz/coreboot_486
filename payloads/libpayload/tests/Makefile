CC=gcc -g -m32
INCLUDES=-I. -I../include -I../include/486
TARGETS=cbfs-x86-test

cbfs-x86-test: cbfs-x86-test.c ../arch/486/rom_media.c ../libcbfs/ram_media.c ../libcbfs/cbfs.c
	$(CC) -o $@ $^ $(INCLUDES)


all: $(TARGETS)

run: all
	for i in $(TARGETS); do ./$$i; done
