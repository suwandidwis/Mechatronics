# for Opengl

CCGL=g++
CFLAGS = -O2 -DSHM -DHZ=100

INC_FILES= $(wildcard *.cpp) $(wildcard *.o)

all:	coba_serial


coba_serial: coba_serial.c
	$(CCGL) $(CFLAGS) $^ -o $@ 

clean:
	rm -rf *.exe
