CC=clang++
LD=../../utils/tools/bin/i686-elf-ld
CFLAGS=-m32 -c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++0x \
		-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-avx -ffreestanding -Os -stdlib=libc++ -fno-builtin -MD \
		-target i686-none-elf
LDFLAGS=-T booter.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/booter.img

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	yasm -f elf -o entry.o entry.asm
	yasm -f elf -o processor/processorasm.o processor/processor.asm
	$(LD) $(LDFLAGS) entry.o processor/processorasm.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I. -include inc.h

clean:
	@rm -rfv *.o
	@rm -rfv */*.o
	@rm -rfv *.d
	@rm -rfv */*.d

-include $(SOURCES:.cpp=.d)
