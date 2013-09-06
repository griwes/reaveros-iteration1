CC=clang++
LD=../utils/tools/bin/x86_64-elf-ld
CFLAGS=$(CFL) -c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++1y \
		-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-avx -ffreestanding -Os -stdlib=libc++ -fno-builtin \
		-mcmodel=kernel -Wno-unused-variable -Wno-unused-parameter -MD -Wno-unused-private-field -ftemplate-depth-512 \
		-target x86_64-none-elf -Wno-unused-function -integrated-as
LDFLAGS=-T kernel.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp" -and -not -name "main.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=builds/kernel.img

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	yasm -f elf64 -o entry.o entry.asm
	yasm -f elf64 -o processor/asm.o processor/processor.asm
	yasm -f elf64 -o processor/trampoline.o processor/trampoline.asm
	$(CC) $(CFLAGS) main.cpp -o main.o -I. -include inc.h
	$(LD) $(LDFLAGS) entry.o processor/asm.o processor/trampoline.o main.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I. -include inc.h

clean:
	@rm -rfv *.o
	@rm -rfv */*.o
	@rm -rfv *.d
	@rm -rfv */*.d

-include $(SOURCES:.cpp=.d)
