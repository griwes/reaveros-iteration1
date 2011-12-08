CC=g++
LD=ld
CFLAGS=-c -Wall -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector
LDFLAGS=-T booter.ld --oformat binary -nostdlib
SOURCES=booter.cpp screen.cpp physmem.cpp main.cpp initrd.cpp string.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=booter.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	nasm -f elf -o bootstrap.o main.asm
	nasm -f elf -o pagingasm.o paging.asm
	$(LD) $(LDFLAGS) bootstrap.o pagingasm.o $(OBJECTS) -o $@ -L/usr/lib/gcc/i686-linux-gnu/4.6/ -lgcc

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
