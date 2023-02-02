/*
------------------------------------------------------------------------------
rand.h: definitions for a random number generator
By Bob Jenkins, 1996, Public Domain
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: renamed seed to flag
  980605: recommend RANDSIZL=4 for noncryptography.
  010626: note this is public domain
------------------------------------------------------------------------------
*/
#ifndef _ISAAC_RAND_H_
#define _ISAAC_RAND_H_

#include "isaacStandard.h"

#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

/* context of random number generator */
typedef struct isaacRandCtx
{
  ub4 randcnt;
  ub4 randrsl[RANDSIZ];
  ub4 randmem[RANDSIZ];
  ub4 randa;
  ub4 randb;
  ub4 randc;
} isaacRandCtx_t;

class CIsaacRand
{
    public:
        static CIsaacRand *getInstance(uint32_t seed);
        static CIsaacRand *getInstance(void);
        static CIsaacRand *getInstanceTOD(void);

        uint32_t ranval(void);
        uint8_t ranbyte(void);
        uint8_t ranbyteRange(uint8_t lower, uint8_t upper);
        void    fillBuffer(uint8_t *buf, uint32_t len);

    private:

        CIsaacRand(uint32_t seed);

        static CIsaacRand *theInstance;

        void init(int flag, uint32_t seed);
        void isaacGen(void);

        isaacRandCtx_t ctx;
};

#endif
