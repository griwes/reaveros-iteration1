CC=g++
LD=ld
CFLAGS=-c -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti \
		-fno-stack-protector -std=c++0x -ffreestanding -mcmodel=kernel -mno-mmx -mno-sse -mno-sse2 \
		-mno-sse3 -mno-3dnow -mno-red-zone
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=main.cpp cxx.cpp \
	processor/processor.cpp processor/synchronization.cpp processor/smp.cpp processor/core.cpp \
	memory/memory.cpp memory/memorymap.cpp memory/paging.cpp memory/heap.cpp memory/vm.cpp \
	lib/string.cpp lib/stack.cpp \
	screen/screen.cpp screen/terminal.cpp screen/console.cpp \
	scheduler/scheduler.cpp \
	acpi/acpi.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/kernel.img

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	yasm -f elf64 -o bootstrap.o entry.asm
	yasm -f elf64 -o processor.o processor/processor.asm
	yasm -f elf64 -o synchronization.o processor/synchronization.asm
	$(LD) $(LDFLAGS) bootstrap.o processor.o synchronization.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -rfv */*.o
	@rm -rfv *.o
