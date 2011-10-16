CC=g++
LD=ld
CFLAGS=-c -Wall -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector
LDFLAGS=-T booter.ld --oformat binary
SOURCES=booter.cpp screen.cpp physmem.cpp main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=booter.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	nasm -f elf -o bootstrap.o main.asm
	$(LD) $(LDFLAGS) bootstrap.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o