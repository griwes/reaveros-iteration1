CC=g++
LD=ld
CFLAGS=-c -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti \
		-fno-stack-protector -std=c++0x -ffreestanding -mcmodel=kernel -mno-mmx -mno-sse -mno-sse2 \
		-mno-sse3 -mno-3dnow -mno-red-zone
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=main.cpp cxx.cpp \
	processor/processor.cpp \
	memory/memory.cpp memory/memorymap.cpp memory/paging.cpp memory/heap.cpp memory/pagestack.cpp memory/vmm.cpp \
	lib/string.cpp \
	screen/screen.cpp screen/terminal.cpp screen/console.cpp \
	scheduler/scheduler.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/kernel.img

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	yasm -f elf64 -o bootstrap.o entry.asm
	yasm -f elf64 -o processor.o processor/processor.asm
	$(LD) $(LDFLAGS) bootstrap.o processor.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -rfv */*.o
	@rm -rfv *.o
