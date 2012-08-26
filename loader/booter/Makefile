CC=clang++
LD=../../utils/tools/bin/i686-elf-ld
CFLAGS=-m32 -c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x \
		-mno-sse -mno-sse2 -mno-sse3 -ffreestanding -O0
LDFLAGS=-T booter.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/booter.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	yasm -f elf -o entry.o entry.asm
	$(LD) $(LDFLAGS) entry.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I. -include inc.h

clean:
	@rm -rfv *.o
