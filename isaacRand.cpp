/*
------------------------------------------------------------------------------
rand.c: By Bob Jenkins.  My random number generator, ISAAC.  Public Domain.
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: added main (ifdef'ed out), also rearranged randinit()
  010626: Note that this is public domain
------------------------------------------------------------------------------
*/
#include "isaacStandard.h"
#include "isaacRand.h"

#include <stdint.h>
#include <string.h>
#include <time.h>


#define ind(mm,x)  (*(ub4 *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<2))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}


#define isaacMix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

CIsaacRand *CIsaacRand::theInstance = (CIsaacRand *)(NULL);

CIsaacRand* CIsaacRand::getInstance(void)
{
    if ((CIsaacRand *)(NULL) == theInstance)
    {
        theInstance = new CIsaacRand(0);
    }
    return theInstance;
}

CIsaacRand* CIsaacRand::getInstance(uint32_t seed)
{
    if ((CIsaacRand *)(NULL) == theInstance)
    {
        theInstance = new CIsaacRand(seed);
    }
    else
    {
        theInstance->init(2, seed);
    }
    return theInstance;
}

CIsaacRand* CIsaacRand::getInstanceTOD(void)
{
    if ((CIsaacRand *)(NULL) == theInstance)
    {
        theInstance = new CIsaacRand((uint32_t)(time(NULL)));
    }
    else
    {
        theInstance->init(2, (uint32_t)(time(NULL)));
    }
    return theInstance;
}

CIsaacRand::CIsaacRand(uint32_t seed)
{
    init(2, seed);
}

uint32_t CIsaacRand::ranval(void)
{
    if (0 == ctx.randcnt--)
    {
        isaacGen();
        ctx.randcnt = RANDSIZ - 1;
    }

    return ctx.randrsl[ctx.randcnt];
}

uint8_t CIsaacRand::ranbyte(void)
{
    return (uint8_t)(ranval() & 0xFF);
}

uint8_t CIsaacRand::ranbyteRange(uint8_t lower, uint8_t upper)
{
    uint8_t r = lower;

    if (upper > lower)
    {
        uint8_t m = upper - lower + 1;
        r = (ranbyte() % m) + lower;
    }
    return r;
}

void CIsaacRand::isaacGen(void)
{
    ub4 a,b,x,y,*m,*mm,*m2,*r,*mend;
    mm=ctx.randmem;
    r=ctx.randrsl;
    a = ctx.randa;
    b = ctx.randb + (++ctx.randc);
    for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; )
    {
        rngstep( a<<13, a, b, mm, m, m2, r, x);
        rngstep( a>>6, a, b, mm, m, m2, r, x);
        rngstep( a<<2, a, b, mm, m, m2, r, x);
        rngstep( a>>16, a, b, mm, m, m2, r, x);
    }
    for (m2 = mm; m2<mend; )
    {
        rngstep( a<<13, a, b, mm, m, m2, r, x);
        rngstep( a>>6, a, b, mm, m, m2, r, x);
        rngstep( a<<2, a, b, mm, m, m2, r, x);
        rngstep( a>>16, a, b, mm, m, m2, r, x);
    }
    ctx.randb = b;
    ctx.randa = a;
}


/* if (flag==TRUE), then use the contents of randrsl[] to initialize mm[]. */
void CIsaacRand::init(int flag, uint32_t seed)
{
    int i;
    ub4 a,b,c,d,e,f,g,h;
    ub4 *m,*r;
    memset((void *)(&ctx), 0, sizeof(isaacRandCtx_t));
    ctx.randa = ctx.randb = ctx.randc = 0;
    m=ctx.randmem;
    r=ctx.randrsl;
    a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

    for (i=0; i<4; ++i)          /* scramble it */
    {
        isaacMix(a,b,c,d,e,f,g,h);
    }

    if (2 == flag)              /* seed w/ time of day */
    {
        for(i=0; i<RANDSIZ; i++)
        {
            r[i] = seed;
        }
    }

    switch (flag)
    {
        case 1:
        case 2:
            /* initialize using the contents of r[] as the seed */
            for (i=0; i<RANDSIZ; i+=8)
            {
                a+=r[i  ];
                b+=r[i+1];
                c+=r[i+2];
                d+=r[i+3];
                e+=r[i+4];
                f+=r[i+5];
                g+=r[i+6];
                h+=r[i+7];
                isaacMix(a,b,c,d,e,f,g,h);
                m[i  ]=a;
                m[i+1]=b;
                m[i+2]=c;
                m[i+3]=d;
                m[i+4]=e;
                m[i+5]=f;
                m[i+6]=g;
                m[i+7]=h;
            }
            /* do a second pass to make all of the seed affect all of m */
            for (i=0; i<RANDSIZ; i+=8)
            {
                a+=m[i  ];
                b+=m[i+1];
                c+=m[i+2];
                d+=m[i+3];
                e+=m[i+4];
                f+=m[i+5];
                g+=m[i+6];
                h+=m[i+7];
                isaacMix(a,b,c,d,e,f,g,h);
                m[i  ]=a;
                m[i+1]=b;
                m[i+2]=c;
                m[i+3]=d;
                m[i+4]=e;
                m[i+5]=f;
                m[i+6]=g;
                m[i+7]=h;
            }
            break;
        case 0:
        default:
            /* fill in m[] with messy stuff */
            for (i=0; i<RANDSIZ; i+=8)
            {
                isaacMix(a,b,c,d,e,f,g,h);
                m[i  ]=a;
                m[i+1]=b;
                m[i+2]=c;
                m[i+3]=d;
                m[i+4]=e;
                m[i+5]=f;
                m[i+6]=g;
                m[i+7]=h;
            }
            break;
    }

    isaacGen();            /* fill in the first set of results */
    ctx.randcnt=RANDSIZ;  /* prepare to use the first set of results */
}

void CIsaacRand::fillBuffer(uint8_t *buf, uint32_t len)
{
    uint32_t *lBuf = (uint32_t *)buf;
    uint8_t *bBuf;
    uint32_t remainderIndex;

    remainderIndex = len / sizeof(uint32_t);
    remainderIndex *= sizeof(uint32_t);
    bBuf = &(buf[remainderIndex]);

    while (len >= sizeof(uint32_t))
    {
        *lBuf++ = ranval();
        len -= sizeof(uint32_t);
    }
    while (len > 0)
    {
        *bBuf++ = (uint8_t)(ranbyte() & 0xFF);
        --len;
    }
}
