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

/* Tests in the test suite that are done for both the default and
** the test configurations.
*/

/* Since this file is included twice, clean up all defines.
*/
#undef U
#undef sz_bau
#undef sz_aau
#undef DIV_ROUND_UP
#undef HEAD_AAUS
#undef PTR_RECORD_AAUS
#undef DUMMY_END_BLOCK_BAUS
#undef MIN_BLOCK_BAUS
#undef MIN_BLOCK_AAUS
#undef MIN_BLOCK_AAUS
#undef MIN_BLOCK_PYLD_AAUS
#undef MANY_BLOCKS
#undef chunk
#undef chunk2
#undef d
#undef p

#include "heapmm.h"

/* Some swell abbreviations. */

#define U(X) HMM_UNIQUE(X)

#define sz_bau U(size_bau)
#define sz_aau U(size_aau)

#define chunk U(chunk)
#define chunk2 U(chunk2)
#define d U(d)
#define p U(p)

char chunk[HMM_BLOCK_ALIGN_UNIT * 11000][HMM_ADDR_ALIGN_UNIT];

char chunk2[HMM_BLOCK_ALIGN_UNIT * 11000][HMM_ADDR_ALIGN_UNIT];

U(descriptor) d;

/* The number of blocks of distinct sizes allocated in the "many
** blocks" test.  402 is used instead of 400 because 402 % 7 == 3.
*/
#define MANY_BLOCKS 402

void *(p[MANY_BLOCKS]);

/* Copy some things from hmm_intnl.h. */

typedef struct
  {
    sz_bau previous_block_size, block_size;
  }
U(head_record);

typedef struct
  {
    struct ptr_struct *self, *prev, *next;
  }
U(ptr_record);

#define DIV_ROUND_UP(NUMER, DENOM) (((NUMER) + (DENOM) - 1) / (DENOM))

#define HEAD_AAUS DIV_ROUND_UP(sizeof(U(head_record)), HMM_ADDR_ALIGN_UNIT)

#define PTR_RECORD_AAUS DIV_ROUND_UP(sizeof(U(ptr_record)), HMM_ADDR_ALIGN_UNIT)

#define DUMMY_END_BLOCK_BAUS DIV_ROUND_UP(HEAD_AAUS, HMM_BLOCK_ALIGN_UNIT)

#define MIN_BLOCK_BAUS \
  DIV_ROUND_UP(HEAD_AAUS + PTR_RECORD_AAUS, HMM_BLOCK_ALIGN_UNIT)

#define MIN_BLOCK_AAUS (MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT)

/* Minimum number of blocks in payload of minimum size block. */
#define MIN_BLOCK_PYLD_AAUS (MIN_BLOCK_AAUS - HEAD_AAUS)

/* Do alloc function and set payload bytes to a known ramping pattern.
*/
void * U(from_heap)(sz_aau n_aaus)
  {
    void *mem = (unsigned char *) U(alloc)(&d, n_aaus);

    if (mem)
      {
	unsigned char *p = (unsigned char *) mem;
	sz_aau true_sz = U(true_size)(mem);
	unsigned char b = 0;

	if (!true_sz)
	  WRONG

	if (true_sz < n_aaus)
	  WRONG

	true_sz *= HMM_ADDR_ALIGN_UNIT;

	do
	  {
	    *(p++) = b++;
	    true_sz--;
	  }
	while (true_sz);
      }

    return(mem);
  }

/* Check block payload for ramping pattern, then free block.
*/
void U(to_heap)(void *mem)
  {
    unsigned char *p = (unsigned char *) mem;
    unsigned char b = 0;
    sz_aau th;

    th = U(true_size)(mem);

    if (!th)
      WRONG

    th *= HMM_ADDR_ALIGN_UNIT;

    do
      {
	if (*(p++) != b++)
	  WRONG
	th--;
      }
    while (th);

    U(free)(&d, mem);
  }

/* Check for ramping, call resize function, redo ramping. */
int U(change_size)(void *mem, sz_aau new_size)
  {
    unsigned char *p = (unsigned char *) mem;
    unsigned char b = 0;
    sz_aau th;

    th = U(true_size)(mem);

    if (!th)
      WRONG

    th *= HMM_ADDR_ALIGN_UNIT;

    do
      {
	if (*(p++) != b++)
	  WRONG
	th--;
      }
    while (th);

    switch (U(resize)(&d, mem, new_size))
      {
      case -1:
	return(-1);

      default:
	WRONG

      case 0:
	{
	  p = (unsigned char *) mem;
	  th = U(true_size)(mem);
	  b = 0;

	  if (!th)
	    WRONG

	  if (th < new_size)
	    WRONG

	  th *= HMM_ADDR_ALIGN_UNIT;

	  do
	    {
	      *(p++) = b++;
	      th--;
	    }
	  while (th);
	}
	break;
      }

    return(0);
  }

/* Tests involving allocation of just one block.
*/
void U(one_block)(void)
  {
    U(init)(&d);

    if (U(from_heap)(0) != 0)
      WRONG

    if (U(largest_available)(&d) != 0)
      WRONG

    U(new_chunk)(&d, chunk, (5 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Allocate the entire chunk at once. */

    if ((p[0] = U(from_heap)((4 * MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    if (U(from_heap)(0) != 0)
      WRONG

    U(to_heap)(p[0]);

    /* Check remainder block logic. */

    if ((p[0] = U(from_heap)((3 * MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			     MIN_BLOCK_PYLD_AAUS + 1)) == 0)
      WRONG

    /* There should not have been a remainder block created. */
    if (U(from_heap)(0) != 0)
      WRONG

    U(to_heap)(p[0]);

    if ((p[0] = U(from_heap)((3 * MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    /* There should have been a remainder block created. */
    if (U(from_heap)(MIN_BLOCK_PYLD_AAUS) == 0)
      WRONG
  }

/* Tests shrinking. */
void U(shrink_test)(void)
  {
    U(init)(&d);

    U(new_chunk)(&d, chunk, (2 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Test shrinking entire chunk out of existence. */

    if ((p[0] = U(from_heap)((MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			     MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);

    if (d.end_of_shrinkable_chunk !=
	(chunk + (HMM_BLOCK_ALIGN_UNIT *
                  ((2 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS))))
      WRONG
    if (d.num_baus_can_shrink != ((2 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS))
      WRONG

    U(shrink_chunk)(&d, d.num_baus_can_shrink);

    if (U(from_heap)(0) != 0)
      WRONG

    U(new_chunk)(&d, chunk, (2 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Maximal shrink of chunk without killing it. */

    if ((p[0] = U(from_heap)((MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			     MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);

    U(shrink_chunk)(&d, MIN_BLOCK_BAUS);

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    if (U(from_heap)(0) != 0)
      WRONG

    U(to_heap)(p[0]);

    U(init)(&d);

    U(new_chunk)(&d, chunk, (3 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Shrink last block out of existence. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)((MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			     MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[1]);

    if (d.end_of_shrinkable_chunk !=
	(chunk + (HMM_BLOCK_ALIGN_UNIT * ((3 * MIN_BLOCK_BAUS) +
	          DUMMY_END_BLOCK_BAUS))))
      WRONG
    if (d.num_baus_can_shrink != (2 * MIN_BLOCK_BAUS))
      WRONG

    U(shrink_chunk)(&d, d.num_baus_can_shrink);

    if (U(from_heap)(0) != 0)
      WRONG
    U(to_heap)(p[0]);

    U(init)(&d);

    U(new_chunk)(&d, chunk, (3 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Shrink last block to minimum. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)((MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) +
			     MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[1]);

    U(shrink_chunk)(&d, MIN_BLOCK_BAUS);

    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    if (U(from_heap)(0) != 0)
      WRONG
    U(to_heap)(p[0]);
    U(to_heap)(p[1]);

    U(init)(&d);

    U(new_chunk)(&d, chunk, (3 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Check logic with a coalescing. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[1]);

    if (d.num_baus_can_shrink)
      WRONG

    U(to_heap)(p[2]);

    if (d.end_of_shrinkable_chunk !=
	(chunk + (HMM_BLOCK_ALIGN_UNIT * ((3 * MIN_BLOCK_BAUS) +
	          DUMMY_END_BLOCK_BAUS))))
      WRONG
    if (d.num_baus_can_shrink != (2 * MIN_BLOCK_BAUS))
      WRONG

    U(to_heap)(p[0]);

    if (d.end_of_shrinkable_chunk !=
	(chunk + (HMM_BLOCK_ALIGN_UNIT * ((3 * MIN_BLOCK_BAUS) +
	          DUMMY_END_BLOCK_BAUS))))
      WRONG
    if (d.num_baus_can_shrink != ((3 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS))
      WRONG
  }

/* Test resize function. */

void U(test_resize)(void)
  {
    printf("Test resize function\n");

    U(init)(&d);

    U(new_chunk)(&d, chunk, (4 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Test expanding resize where the next block is not last freed. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);

    if (U(change_size)(p[1], MIN_BLOCK_PYLD_AAUS + 1) != 0)
      WRONG

    if (U(largest_available)(&d) !=
        (MIN_BLOCK_PYLD_AAUS + MIN_BLOCK_AAUS - HMM_BLOCK_ALIGN_UNIT))
      WRONG

    U(to_heap)(p[1]);

    U(shrink_chunk)(&d, (4 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    if (U(largest_available)(&d) != 0)
      WRONG

    U(init)(&d);

    U(new_chunk)(&d, chunk, (4 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);

    if (U(change_size)(p[1], MIN_BLOCK_PYLD_AAUS + MIN_BLOCK_AAUS) != 0)
      WRONG

    if (U(largest_available)(&d) != MIN_BLOCK_PYLD_AAUS)
      WRONG

    U(to_heap)(p[1]);

    U(shrink_chunk)(&d, (4 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    if (U(largest_available)(&d) != 0)
      WRONG

    U(init)(&d);

    U(new_chunk)(&d, chunk, (3 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (2 * MIN_BLOCK_AAUS) + 1)
        != -1)
      WRONG
    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (2 * MIN_BLOCK_AAUS)) != 0)
      WRONG

    if (U(largest_available)(&d) != 0)
      WRONG

    U(to_heap)(p[0]);

    U(shrink_chunk)(&d, (3 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    if (U(largest_available)(&d) != 0)
      WRONG

    U(init)(&d);

    U(new_chunk)(&d, chunk, (4 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    /* Test expanding resize where the next block is last freed. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);
    U(to_heap)(p[2]);

    if (U(change_size)(p[1], MIN_BLOCK_PYLD_AAUS + 1) != 0)
      WRONG

    if (U(largest_available)(&d) !=
        (MIN_BLOCK_PYLD_AAUS + MIN_BLOCK_AAUS - HMM_BLOCK_ALIGN_UNIT))
      WRONG

    U(to_heap)(p[1]);

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);
    U(to_heap)(p[2]);

    if (U(change_size)(p[1], MIN_BLOCK_PYLD_AAUS + MIN_BLOCK_AAUS) != 0)
      WRONG

    if (U(largest_available)(&d) != MIN_BLOCK_PYLD_AAUS)
      WRONG

    U(to_heap)(p[1]);

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[1]);

    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (3 * MIN_BLOCK_AAUS) + 1)
        != -1)
      WRONG
    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (3 * MIN_BLOCK_AAUS)) != 0)
      WRONG

    if (U(largest_available)(&d) != 0)
      WRONG

    /* Attempt to expand size of block past free block after. */
    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (3 * MIN_BLOCK_AAUS) + 1)
        != -1)
      WRONG

    /* Test reducing size of block with no free block after. */

    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (3 * MIN_BLOCK_AAUS) - 1)
        != 0)
      WRONG

    if (U(largest_available)(&d) != 0)
      WRONG

    /* Attempt to expand size of block when no free block after. */
    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (3 * MIN_BLOCK_AAUS) + 1)
        != -1)
      WRONG

    if (U(change_size)(p[0], MIN_BLOCK_PYLD_AAUS + (2 * MIN_BLOCK_AAUS))
        != 0)
      WRONG

    if (U(largest_available)(&d) != MIN_BLOCK_PYLD_AAUS)
      WRONG

    U(to_heap)(p[0]);

    /* Test reducing size of with last freed block after. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS + MIN_BLOCK_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    if (U(change_size)(p[1], MIN_BLOCK_PYLD_AAUS - 1) != 0)
      WRONG

    if (U(true_size)(p[1]) != MIN_BLOCK_PYLD_AAUS)
      WRONG

    U(to_heap)(p[1]);

    if (U(change_size)(p[0], 0) != 0)
      WRONG

    if (U(true_size)(p[0]) != MIN_BLOCK_PYLD_AAUS)
      WRONG

    U(to_heap)(p[0]);

    /* Test reducing size of with freed block after not last freed. */

    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(MIN_BLOCK_PYLD_AAUS + MIN_BLOCK_AAUS)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[2]);
    U(to_heap)(p[0]);

    if (U(change_size)(p[1], 0) != 0)
      WRONG

    if (U(true_size)(p[1]) != MIN_BLOCK_PYLD_AAUS)
      WRONG

    U(to_heap)(p[1]);

    U(shrink_chunk)(&d, (4 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    if (U(largest_available)(&d) != 0)
      WRONG
  }

/* Allocate 3 blocks of three given sizes.
*/
void U(alloc3)(sz_aau b1, sz_aau b2, sz_aau b3)
  {
    printf("alloc3: %d %d %d\n", (unsigned) b1, (unsigned) b2, (unsigned) b3);

    if ((p[0] = U(from_heap)(b1)) == 0)
      WRONG
    if ((p[1] = U(from_heap)(b2)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(b3)) == 0)
      WRONG
  }

/* Free 3 blocks whose pointers are at the given indexes, in the given
** order.
*/
void U(free3)(int idx1, int idx2, int idx3)
  {
    U(to_heap)(p[idx1]);
    U(to_heap)(p[idx2]);
    U(to_heap)(p[idx3]);
  }

/* Test details coalescing logic by allocating and freeing 3 adjacent
** blocks in different orders.  Also tests growing chunk.
*/
void U(three_blocks)(sz_aau b1, sz_aau b2, sz_aau b3)
  {
    sz_bau chunk_size =
      (sz_bau) (DIV_ROUND_UP(b1 + HEAD_AAUS, HMM_BLOCK_ALIGN_UNIT) +
                DUMMY_END_BLOCK_BAUS);

    U(init)(&d);

    U(new_chunk)(&d, chunk, chunk_size);

    if ((p[0] = U(from_heap)(b1)) == 0)
      WRONG

    /* Test growing chunk after allocated block. */

    U(grow_chunk)(
      &d, chunk + (chunk_size * HMM_BLOCK_ALIGN_UNIT), MIN_BLOCK_BAUS);

    chunk_size += MIN_BLOCK_BAUS;

    if ((p[1] = U(from_heap)((MIN_BLOCK_BAUS * HMM_BLOCK_ALIGN_UNIT) -
                             HEAD_AAUS)) == 0)
      WRONG

    if (U(from_heap)(0) != 0)
      WRONG

    U(to_heap)(p[1]);

    /* Test growing chunk after free block. */

    U(grow_chunk)(
      &d, chunk + (chunk_size * HMM_BLOCK_ALIGN_UNIT),
      DIV_ROUND_UP(b2 + HEAD_AAUS, HMM_BLOCK_ALIGN_UNIT) +
	DIV_ROUND_UP(b3 + HEAD_AAUS, HMM_BLOCK_ALIGN_UNIT) -
	MIN_BLOCK_BAUS);

    if ((p[1] = U(from_heap)(b2)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(b3)) == 0)
      WRONG
    if (U(from_heap)(0) != 0)
      WRONG

    U(free3)(0, 1, 2);

    /* Test every possible order for freeing the three blocks.  This
    ** will test all branches in coalesing logic.
    */

    U(alloc3)(b1, b2, b3);
    U(free3)(0, 1, 2);
    U(alloc3)(b1, b2, b3);
    U(free3)(0, 2, 1);
    U(alloc3)(b1, b2, b3);
    U(free3)(1, 0, 2);
    U(alloc3)(b1, b2, b3);
    U(free3)(1, 2, 0);
    U(alloc3)(b1, b2, b3);
    U(free3)(2, 0, 1);
    U(alloc3)(b1, b2, b3);
    U(free3)(2, 1, 0);
  }

/* The "long bins" test exercises all branches in the logic to take
** an adjacent free block out of the free collection.  For each step
** in the test, 11 blocks are allocated (identified by their index in
** the pointer array, 0 through 10).  All of the even blocks are then
** freed.  This creates a free collection with a single bin with 6
** blocks in it.  Each step of the test consists of freeing blocks
** 1, 5 and 9 in a different order.  Thus, it will hit the case
** where an adjacent block is not the first block in the bin, the
** first block in a multi-block bin, and the only block in its bin.
*/

void U(long_bins_step)(int idx1, int idx2, int idx3)
  {
    sz_aau i;

    printf("long bins step: %d %d %d\n", idx1, idx2, idx3);

    for (i = 0; i < 11; i++)
      {
	if ((p[i] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
	  WRONG
      }

    /* Test all branches in alloc_from_bin. */

    U(to_heap)(p[0]);
    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);
    U(to_heap)(p[2]);
    if ((p[2] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);
    U(to_heap)(p[2]);
    U(to_heap)(p[4]);
    if ((p[4] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[2] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG
    if ((p[0] = U(from_heap)(MIN_BLOCK_PYLD_AAUS)) == 0)
      WRONG

    U(to_heap)(p[0]);
    U(to_heap)(p[2]);
    U(to_heap)(p[4]);
    U(to_heap)(p[6]);
    U(to_heap)(p[8]);
    U(to_heap)(p[10]);

    U(to_heap)(p[idx1]);
    U(to_heap)(p[idx2]);
    U(to_heap)(p[idx3]);

    U(to_heap)(p[3]);
    U(to_heap)(p[7]);
  }

void U(long_bins)(void)
  {
    printf("long bins\n");

    U(init)(&d);

    U(new_chunk)(&d, chunk, (11 * MIN_BLOCK_BAUS) + DUMMY_END_BLOCK_BAUS);

    U(long_bins_step)(1, 5, 9);
    U(long_bins_step)(1, 9, 5);
    U(long_bins_step)(5, 1, 9);
    U(long_bins_step)(5, 9, 1);
    U(long_bins_step)(9, 1, 5);
    U(long_bins_step)(9, 5, 1);
  }

/* Test with lots of free blocks and a (fairly) big AVL tree.  Also
** tests more than one chunk.
*/
void U(many_blocks)(void)
  {
    int chunk_added = 0;
    sz_aau i;

    U(init)(&d);

    U(new_chunk)(&d, chunk, 11000);

    printf("Allocating many blocks\n");

    for (i = 0; i < MANY_BLOCKS; i++)
      {
	if ((p[i] = U(from_heap)((i / 4) * HMM_BLOCK_ALIGN_UNIT)) == 0)
	  {
	    if (chunk_added)
	      WRONG

	    printf("Adding 2nd chunk\n");
	    U(new_chunk)(&d, chunk2, 11000);
	    chunk_added = 1;

	    if ((p[i] = U(from_heap)((i / 4) * HMM_BLOCK_ALIGN_UNIT)) == 0)
	      WRONG
	  }
      }

    printf("Freeing some blocks\n");

    /* Now I'm going to use the fact that 7 is a generator of the ring
    ** of integers mod MANY_BLOCKS.  Since MANY_BLOCKS mod 7 == 3, the
    ** generating loop will not free adjacent blocks the second time
    ** through, the way it would if MANY_BLOCKS mod 7 == 1.  This
    ** keeps blocks from coalaescing and increases the size of the free
    ** collection.
    */

    i = 0;
    do
      {
	i += 7;
	if (i >= MANY_BLOCKS)
	  i -= MANY_BLOCKS;

	U(to_heap)(p[i]);
      }
    while (i != 194);

    printf("Allocating blocks again\n");

    i = 0;
    do
      {
	i += 7;
	if (i >= MANY_BLOCKS)
	  i -= MANY_BLOCKS;

	if ((p[i] = U(from_heap)((i / 4) * HMM_BLOCK_ALIGN_UNIT)) == 0)
	  WRONG
      }
    while (i != 194);

    printf("Freeing all blocks\n");

    i = 0;
    do
      {
	i += 7;
	if (i >= MANY_BLOCKS)
	  i -= MANY_BLOCKS;

	U(to_heap)(p[i]);
      }
    while (i);

    /* Make sure I'm really doing best-fit. */

    printf("largest=%u\n", (unsigned) U(largest_available)(&d));

    if (U(largest_available)(&d) < 10500)
      WRONG

    if (U(from_heap)(10500) == 0)
      WRONG

    printf("largest=%u\n", (unsigned) U(largest_available)(&d));

    if (U(largest_available)(&d) < 10500)
      WRONG

    if (U(from_heap)(10500) == 0)
      WRONG
  }

void U(do_tests)(void)
  {
    U(one_block)();

    U(shrink_test)();

    U(test_resize)();

    /* Do the three blocks test with lots of different relative block
    ** sizes.
    */
    U(three_blocks)(
      MIN_BLOCK_PYLD_AAUS, MIN_BLOCK_PYLD_AAUS, MIN_BLOCK_PYLD_AAUS);
    U(three_blocks)(
      MIN_BLOCK_PYLD_AAUS, MIN_BLOCK_PYLD_AAUS + 1, MIN_BLOCK_PYLD_AAUS + 2);
    U(three_blocks)(1000, 1000, 1000);
    U(three_blocks)(1001, 1002, 1003);
    U(three_blocks)(MIN_BLOCK_PYLD_AAUS, 1000, 1500);

    U(long_bins)();

    U(many_blocks)();
  }
