CC=clang++
LD=../../utils/tools/bin/x86_64-elf-ld
CFLAGS=$(CFL) -c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++1y \
		-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-avx -ffreestanding -Os -stdlib=libc++ -fno-builtin \
		-Wno-unused-variable -Wno-unused-parameter -MD -Wno-unused-private-field \
		-target x86_64-none-elf -Wno-unused-function -integrated-as -Weffc++
LDFLAGS=-T initsrv.ld --oformat binary -nostdlib
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=init.srv

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) main.o
	yasm -f elf64 -o entry.o entry.asm
	$(LD) $(LDFLAGS) entry.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I. -I../../library

clean:
	@rm -rfv *.o
	@rm -rfv */*.o
	@rm -rfv *.d
	@rm -rfv */*.d

-include $(SOURCES:.cpp=.d)
