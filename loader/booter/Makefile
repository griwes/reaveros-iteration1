CC=g++
LD=ld
CFLAGS=-m32 -c -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x
LDFLAGS=-mi386linux -T booter.ld --oformat binary -nostdlib
SOURCES=main.cpp screen.cpp memory.cpp initrd.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/booter.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	nasm -f elf -o bootstrap.o entry.asm
	$(LD) $(LDFLAGS) bootstrap.o $(OBJECTS) -o $@ -L/usr/lib/gcc/x86_64-linux-gnu/4.6/32/ -lgcc

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
