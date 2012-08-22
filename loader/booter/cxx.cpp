#include <cstdint>
#include <cstddef>

inline void * operator new(uint32_t, void * addr)
{
    return addr;
}
