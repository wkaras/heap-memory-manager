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

#ifndef HMM_INTRNL_H_
#define HMM_INTRNL_H_

#include "heapmm.h"

#undef U
#define U(BASE) HMM_UNIQUE(BASE)

/* Mask of high bit of variable of size_bau type. */
#define HIGH_BIT_BAU_SIZE \
  ((U(size_bau)) ~ (((U(size_bau)) ~ (U(size_bau)) 0) >> 1))

/* Add a given number of AAUs to pointer. */
#define AAUS_FORWARD(PTR, AAU_OFFSET) \
  (((char *) (PTR)) + ((AAU_OFFSET) * ((U(size_aau)) HMM_ADDR_ALIGN_UNIT)))

/* Subtract a given number of AAUs from pointer. */
#define AAUS_BACKWARD(PTR, AAU_OFFSET) \
  (((char *) (PTR)) - ((AAU_OFFSET) * ((U(size_aau)) HMM_ADDR_ALIGN_UNIT)))

/* Add a given number of BAUs to a pointer. */
#define BAUS_FORWARD(PTR, BAU_OFFSET) \
  AAUS_FORWARD((PTR), (BAU_OFFSET) * ((U(size_aau)) HMM_BLOCK_ALIGN_UNIT))

/* Subtract a given number of BAUs to a pointer. */
#define BAUS_BACKWARD(PTR, BAU_OFFSET) \
  AAUS_BACKWARD((PTR), (BAU_OFFSET) * ((U(size_aau)) HMM_BLOCK_ALIGN_UNIT))

typedef struct head_struct
  {
    /* Sizes in Block Alignment Units. */
    HMM_UNIQUE(size_bau) previous_block_size, block_size;
  }
head_record;

typedef struct ptr_struct
  {
    struct ptr_struct *self, *prev, *next;
  }
ptr_record;

/* Divide and round up any fraction to the next whole number. */
#define DIV_ROUND_UP(NUMER, DENOM) (((NUMER) + (DENOM) - 1) / (DENOM))

/* Number of AAUs in a block head. */
#define HEAD_AAUS DIV_ROUND_UP(sizeof(head_record), HMM_ADDR_ALIGN_UNIT)

/* Number of AAUs in a block pointer record. */
#define PTR_RECORD_AAUS DIV_ROUND_UP(sizeof(ptr_record), HMM_ADDR_ALIGN_UNIT)

/* Number of BAUs in a dummy end record (at end of chunk). */
#define DUMMY_END_BLOCK_BAUS DIV_ROUND_UP(HEAD_AAUS, HMM_BLOCK_ALIGN_UNIT)

/* Minimum number of BAUs in a block (allowing room for the pointer record. */
#define MIN_BLOCK_BAUS \
  DIV_ROUND_UP(HEAD_AAUS + PTR_RECORD_AAUS, HMM_BLOCK_ALIGN_UNIT)

/* Return number of BAUs in block (masking off high bit containing block
** status). */
#define BLOCK_BAUS(HEAD_PTR) \
  (((head_record *) (HEAD_PTR))->block_size & ~HIGH_BIT_BAU_SIZE)

/* Return number of BAUs in previous block (masking off high bit containing
** block status). */
#define PREV_BLOCK_BAUS(HEAD_PTR) \
  (((head_record *) (HEAD_PTR))->previous_block_size & ~HIGH_BIT_BAU_SIZE)

/* Set number of BAUs in previous block, preserving high bit containing
** block status. */
#define SET_PREV_BLOCK_BAUS(HEAD_PTR, N_BAUS) \
  { register head_record *h_ptr = (head_record *) (HEAD_PTR); \
    h_ptr->previous_block_size &= HIGH_BIT_BAU_SIZE; \
    h_ptr->previous_block_size |= (N_BAUS); }

/* Convert pointer to pointer record of block to pointer to block's head
** record. */
#define PTR_REC_TO_HEAD(PTR_REC_PTR) \
  ((head_record *) AAUS_BACKWARD(PTR_REC_PTR, HEAD_AAUS))

/* Convert pointer to block head to pointer to block's pointer record. */
#define HEAD_TO_PTR_REC(HEAD_PTR) \
  ((ptr_record *) AAUS_FORWARD(HEAD_PTR, HEAD_AAUS))

/* Returns non-zero if block is allocated. */
#define IS_BLOCK_ALLOCATED(HEAD_PTR) \
  (((((head_record *) (HEAD_PTR))->block_size | \
     ((head_record *) (HEAD_PTR))->previous_block_size) & \
    HIGH_BIT_BAU_SIZE) == 0)

#define MARK_BLOCK_ALLOCATED(HEAD_PTR) \
  { register head_record *h_ptr = (head_record *) (HEAD_PTR); \
    h_ptr->block_size &= ~HIGH_BIT_BAU_SIZE; \
    h_ptr->previous_block_size &= ~HIGH_BIT_BAU_SIZE; }

/* Mark a block as free when it is not the first block in a bin (and
** therefore not a node in the AVL tree). */
#define MARK_SUCCESSIVE_BLOCK_IN_FREE_BIN(HEAD_PTR) \
  { register head_record *h_ptr = (head_record *) (HEAD_PTR); \
    h_ptr->block_size |= HIGH_BIT_BAU_SIZE; }

/* Prototypes for internal functions implemented in one file and called in
** another.
*/

void U(into_free_collection)(U(descriptor) *desc, head_record *head_ptr);

void U(out_of_free_collection)(U(descriptor) *desc, head_record *head_ptr);

void * U(alloc_from_bin)(
  U(descriptor) *desc, ptr_record *bin_front_ptr, U(size_bau) n_baus);

#ifdef HMM_AUDIT_FAIL

/* Simply contains a reference to the HMM_AUDIT_FAIL macro and a
** dummy return. */
int U(audit_block_fail_dummy_return)(void);

/* More sickness needed because C has no inline function (yes, it's the
** "use the comma operator like a semicolon" thing.)
*/

/* Auditing a block consists of checking that the size in its head
** matches the previous block size in the head of the next block. */
#define AUDIT_BLOCK_AS_EXPR(HEAD_PTR) \
  ((BLOCK_BAUS(HEAD_PTR) == \
    PREV_BLOCK_BAUS(BAUS_FORWARD(HEAD_PTR, BLOCK_BAUS(HEAD_PTR)))) ? \
   0 : U(audit_block_fail_dummy_return)())

#define AUDIT_BLOCK(HEAD_PTR) \
  { void *h_ptr = (HEAD_PTR); AUDIT_BLOCK_AS_EXPR(h_ptr); }

#endif

/* Interface to AVL tree generic package instantiation. */

#define AVL_UNIQUE(BASE) U(avl_ ## BASE)

#define AVL_HANDLE ptr_record *

#define AVL_KEY U(size_bau)

#define AVL_MAX_DEPTH 64

#include "cavl_if.h"

#endif /* Include once. */
