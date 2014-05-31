CC=clang++
AR=ar
CFLAGS=$(CFL) -c -Wall -Wextra -pedantic -Werror -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -std=c++1y \
		-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-avx -ffreestanding -Os -stdlib=libc++ -fno-builtin \
		-Wno-unused-variable -Wno-unused-parameter -MD -Wno-unused-private-field \
		-target x86_64-none-elf -Wno-unused-function -integrated-as -Weffc++
SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=librose.a

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) crv $@ $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I.

clean:
	@rm -rfv *.o
	@rm -rfv */*.o
	@rm -rfv *.d
	@rm -rfv */*.d

-include $(SOURCES:.cpp=.d)
