CC=clang++
LD=../utils/tools/bin/x86_64-elf-ld
CFLAGS=-c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x \
		-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-avx -ffreestanding -Os -stdlib=libc++ -fno-builtin \
		-Wno-unused-private-field -mcmodel=kernel
# 		^ eh
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/kernel.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	yasm -f elf64 -o entry.o entry.asm
	yasm -f elf64 -o processor/asm.o processor/processor.asm
	$(LD) $(LDFLAGS) entry.o processor/asm.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I. -include inc.h

clean:
	@rm -rfv *.o
	@rm -rfv */*.o
