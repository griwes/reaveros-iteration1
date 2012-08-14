#CC=../../utils/tools/bin/i686-elf-g++
CC=clang++
LD=../../utils/tools/bin/i686-elf-ld
CFLAGS=-m32 -c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x \
		-mno-sse -mno-sse2 -mno-sse3 -ffreestanding -O0
LDFLAGS=-T booter.ld --oformat binary -nostdlib
SOURCES=main.cpp screen.cpp memory.cpp processor.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/booter.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	yasm -f elf -o bootstrap.o entry.asm
	yasm -f elf -o processorasm.o processor.asm
	$(LD) $(LDFLAGS) bootstrap.o processorasm.o $(OBJECTS) -o $@ -L../../utils/tools/lib/gcc/i686-elf/4.7.1/ -lgcc

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -rfv *.o
