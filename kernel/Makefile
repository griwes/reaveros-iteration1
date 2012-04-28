CC=g++
LD=ld
CFLAGS=-c -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x -ffreestanding -mcmodel=large -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=main.cpp processor/processor.cpp memory/memory.cpp memory/memorymap.cpp lib/string.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/kernel.img

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	nasm -f elf64 -o bootstrap.o entry.asm
	$(LD) $(LDFLAGS) bootstrap.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -rfv */*.o
	@rm -rfv *.o
