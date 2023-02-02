#include "naclSupport.h"
#include <stdlib.h>

#if 1
#include "isaacRand.h"
void randombytes(uint8_t *data, uint64_t n)
{
    // Csmallprng::getInstance()->fillBuffer(data, (uint32_t)n);
    CIsaacRand::getInstance()->fillBuffer(data, (uint32_t)n);
}

void randombytes_buf(uint8_t *data, size_t n)
{
    // Csmallprng::getInstance()->fillBuffer(data, (uint32_t)n);
    CIsaacRand::getInstance()->fillBuffer(data, (uint32_t)n);
}

#else

void randombytes(uint8_t *data, uint64_t n)
{
    while (n)
    {
        *data++ = (uint8_t)(rand() % 0xFF);
        n--;
    }
}

void randombytes_buf(uint8_t *data, size_t n)
{
    return randombytes(data, (uint64_t)n);
}

#endif
