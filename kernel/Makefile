CC=clang++
LD=../utils/tools/bin/x86_64-elf-ld
CFLAGS=-c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x \
		-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-avx -ffreestanding -O3 -stdlib=libc++
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/kernel.img

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	yasm -f elf64 -o entry.o entry.asm
	$(LD) $(LDFLAGS) entry.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ # -I. -include inc.h

clean:
	@rm -rfv *.o
	@rm -rfv */*.o
