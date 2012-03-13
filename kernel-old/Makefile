CC=g++
LD=ld
CFLAGS=-c -Wall -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=main.cpp arch/x86/paging.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=kernel.img

all: x86

x86: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	nasm -f elf -o entry.o main.asm
	$(LD) $(LDFLAGS) entry.o $(OBJECTS) -o $@ -L/usr/lib/gcc/i686-linux-gnu/4.6/ -lgcc

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
