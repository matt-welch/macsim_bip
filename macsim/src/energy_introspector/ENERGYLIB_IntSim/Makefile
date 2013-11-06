CC = g++
OFLAGS = -O3 -m32 -DMIN_SYSCALL_MODE -DUSE_SSE_MOVE -Wall -static -fexpensive-optimizations -mtune=core2 -march=core2 -msse4a -mfpmath=sse -funroll-loops

default: main

main:	main.o intsim.o chip.o parameters.o
	$(CC) $(CFLAGS) -o main main.o intsim.o chip.o parameters.o

chip.o:	chip.cc chip.h parameters.h
	$(CC) $(CFLAGS) -c chip.cc

parameters.o:	parameters.cc parameters.h
		$(CC) $(CFLAGS) -c parameters.cc

intsim.o:	intsim.cc intsim.h
		$(CC) $(CFLAGS) -c intsim.cc

main.o:		main.cc intsim.h
		$(CC) $(CFLAGS) -c main.cc

clean:
	rm -f main *.o *~

