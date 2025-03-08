/* Version: 1.1
**
** Copyright (c) 2002-2016 Walter William Karas
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

/* This is a very crude program for comparing the speed of my heap manager
** to the speed of the malloc/free functions in the C standard library.
** It is crude because it uses elapsed time rather than CPU time.  Run
** the program several times on a "quiet" system.  Throw out anomolous
** results.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "heapmm.h"

/* COUNT, HALF_SAME, MIN and STEP are the parameters for the test.
** Try differenct values for them to see how it affects relative
** performance.
*/

/* COUNT is the number of distinct block sizes that will be allocated.
** COUNT must be greater than 13 and cannot have 13 as prime factor. */
#define COUNT 100

/* Half of the number of repeated allocations of each block size.  That is,
** if HALF_SIZE is 10, then 20 blocks will be allocated of each block size.
** If HALF_SIZE is 10 and COUNT is 50, the highest number of blocks
** all allocated at the same time will be 20 * 50 = 1000.
*/
#define HALF_SAME 15

/* The minimum block size. */
#define MIN 3

/* The step between block sizes. */
#define STEP 1

/* Pointers to allocated blocks, and other scratch variables. */
unsigned hmm_rep, std_rep, sz, i;
void *(ptrs[2 * HALF_SAME * COUNT]), **p, *big;

/* Double the number of actually allocated AAUs, to allow for overhead. */
#define UNADJ_BIG_SIZE \
  (2 * HALF_SAME * COUNT * ((2 * MIN) + ((COUNT - 1) * STEP)))

/* Adjust (round up) so size is a whole number of BAUs. */
#define BIG_SIZE \
  (((UNADJ_BIG_SIZE + HMM_BLOCK_ALIGN_UNIT - 1) / HMM_BLOCK_ALIGN_UNIT) * \
   HMM_BLOCK_ALIGN_UNIT)

/* END - STEP is the largest block size allocated. */
#define END (MIN + (COUNT * STEP))

#if (COUNT % 13) == 0
#error COUNT must not be multiple of 13
#endif

/* For some reason, the time() function is very slow and inaccurate in DJGPP,
** so the results of this program are not very useful with that compiler.
*/

time_t base_tm;

void start_timer(void)
  {
    /* Wait for a second transition to make the time interval more
    ** consistent. */
    base_tm = time(0) + 1;
    while (time(0) < base_tm)
      ;
  }

/* Retruns true 10 seconds after start_timer called. */
int time_is_up(void) { return((time(0) - base_tm) >= 10); }

hmm_descriptor d;

/* Flags if in HMM speed test. */
int in_hmm = 0;

/* Check pointer, bail if null, return pointer if not. */
void * nonull(void *p)
  {
    if (!p)
      {
	printf(in_hmm ? "BAIL in hmm\n" : "BAIL in std\n");
	exit(1);
      }
    return(p);
  }

int main(void)
  {
    /* A potential source of "unfairness" is that malloc and free
    ** sometimes have to get memory from the OS and give it back,
    ** something that HMM does not do.  To try to avoid this, allocate
    ** a big block of memory, as big as all the allocations that will
    ** be done.  Then allocate a little "anchor" block.  Then free
    ** the big block.  The anchor block will hopefully stop the free
    ** function from given memory back to the OS when the big block is
    ** freed.
    */

    big = nonull(malloc(BIG_SIZE * HMM_ADDR_ALIGN_UNIT));

    /* Anchor block. */
    malloc(1);

    /* Try to get all the memory we're going to use paged in. */
    memset(big, 0, BIG_SIZE * HMM_ADDR_ALIGN_UNIT);

    free(big);

    std_rep = 0;

    start_timer();

    /* Loop for 10 seconds. */
    do
      {
	/* Allocate all the blocks.  We make use of the fact that 13 is
	** a generator of the ring of integers mod COUNT.  This trick is
	** to avoid having the size be monotonically increasing.
	*/
	p = ptrs;
	sz = MIN;
	do
	  {
	    sz += 13 * STEP;
	    if (sz >= END)
	      sz -= END - MIN;
	    for (i = 0; i < (2 * HALF_SAME); i++)
	      *(p++) = nonull(malloc(sz * HMM_ADDR_ALIGN_UNIT));
	  }
	while (sz != MIN);

        /* Free every other block. */
	p = ptrs;
	sz = COUNT * HALF_SAME;
	do
	  {
	    free(*p);
	    p += 2;
	  }
	while (--sz);

	/* Allocate again the blocks that were freed. */
	p = ptrs;
	sz = MIN;
	do
	  {
	    sz += 13 * STEP;
	    if (sz >= END)
	      sz -= END - MIN;
	    for (i = 0; i < HALF_SAME; i++)
	      {
		*p = nonull(malloc(sz * HMM_ADDR_ALIGN_UNIT));
		p += 2;
	      }
	  }
	while (sz != MIN);

	/* Free every other block. */
	p = ptrs;
	sz = COUNT * HALF_SAME;
	do
	  {
	    free(*p);
	    p += 2;
	  }
	while (--sz);

	/* Free remaining blocks. */
	p = ptrs + 1;
	sz = COUNT * HALF_SAME;
	do
	  {
	    free(*p);
	    p += 2;
	  }
	while (--sz);

	/* Count the number of times we can repeat this sequence in
	** 10 seconds. */
	std_rep++;
      }
    while (!time_is_up());

    /* Now do the same thing over again using the HMM routines. */

    hmm_init(&d);

    big = nonull(malloc(BIG_SIZE * HMM_ADDR_ALIGN_UNIT));
    hmm_new_chunk(&d, big, BIG_SIZE / HMM_BLOCK_ALIGN_UNIT);

    /* Flag that we are now timing the HMM routines. */
    in_hmm = 1;

    hmm_rep = 0;

    start_timer();

    do
      {
	p = ptrs;
	sz = MIN;
	do
	  {
	    sz += 13 * STEP;
	    if (sz >= END)
	      sz -= END - MIN;
	    for (i = 0; i < (2 * HALF_SAME); i++)
	      *(p++) = nonull(hmm_alloc(&d, sz));
	  }
	while (sz != MIN);

	p = ptrs;
	sz = COUNT * HALF_SAME;
	do
	  {
	    hmm_free(&d, *p);
	    p += 2;
	  }
	while (--sz);

	p = ptrs;
	sz = MIN;
	do
	  {
	    sz += 13 * STEP;
	    if (sz >= END)
	      sz -= END - MIN;
	    for (i = 0; i < HALF_SAME; i++)
	      {
		*p = nonull(hmm_alloc(&d, sz));
		p += 2;
	      }
	  }
	while (sz != MIN);

	p = ptrs;
	sz = COUNT * HALF_SAME;
	do
	  {
	    hmm_free(&d, *p);
	    p += 2;
	  }
	while (--sz);

	p = ptrs + 1;
	sz = COUNT * HALF_SAME;
	do
	  {
	    hmm_free(&d, *p);
	    p += 2;
	  }
	while (--sz);

	hmm_rep++;
      }
    while (!time_is_up());

    /* Whoever has the most repetitions in 10 seconds is fastest. */
    printf("std reps: %u\n", std_rep);
    printf("hmm reps: %u\n", hmm_rep);
    printf("average alloc bytes: %u\n", (unsigned) ((MIN + (((COUNT - 1) * STEP) / 2)) * HMM_ADDR_ALIGN_UNIT));

    return(0);
  }
