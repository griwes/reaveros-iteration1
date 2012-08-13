CC=clang++
#CC=../utils/tools/bin/x86_64-elf-g++
LD=../utils/tools/bin/x86_64-elf-ld
CFLAGS=-c -Wall -Wextra -pedantic -Werror -nostdlib -fno-builtin -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x \
		-ffreestanding -mcmodel=large -mno-mmx -mno-sse -mno-sse2 \
		-mno-sse3 -mno-3dnow -mno-red-zone
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=./builds/kernel.img

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
