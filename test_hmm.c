/* This code is in the public domain.
** Version: 1.1  Author: Walt Karas
*/

/* The test suite for the Heap Memory Manager.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#define WRONG wrong(__LINE__);
void wrong(unsigned line)
  {
    printf("WRONG: line=%u\n", line);
    fflush(stdout);
    exit(1);
  }

/* Header file containing code that is executed twice, once for default
** configuration and once for test configuration (configuration 0).
*/
#include "test_hmm.h"

/* Under the test configuration, if an audit failure occurs, a long jump
** is done.  Here are the variables needed for audit failure handling
** under the test configuration.
*/
jmp_buf thmm_jmp_buf;
const char * thmm_fail_file;
unsigned thmm_fail_line;

#define HMM_CNFG_NUM 0

#include "test_hmm.h"

/* Prepare for a test where an audit failure is purposely caused. */
void audit_fail_test_prep(void)
  {
    if (setjmp(thmm_jmp_buf) == 0)
      {
	/* Set up a heap with three free blocks separated by two
	** allocated blocks, with (monotonically increasing) different
	** block sizes.
	*/

	thmm_init(&d);
	thmm_new_chunk(&d, chunk, 100);

	if ((p[0] = thmm_from_heap(10)) == 0)
	  WRONG
	if ((p[1] = thmm_from_heap(11)) == 0)
	  WRONG
	if ((p[2] = thmm_from_heap(12)) == 0)
	  WRONG
	if ((p[3] = thmm_from_heap(13)) == 0)
	  WRONG
	if ((p[4] = thmm_from_heap(14)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
	thmm_to_heap(p[2]);
	thmm_to_heap(p[4]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }
  }

/* Function that corrupts all the bytes in a block head given a pointer
** to the block's payload.
*/
void whack_head(void *pp)
  {
    memset(((char *) pp) - (HEAD_AAUS * HMM_ADDR_ALIGN_UNIT), 0xA5,
           HEAD_AAUS * HMM_ADDR_ALIGN_UNIT);
  }

void audit_fail_shrink_tests(void)
  {
    /* Corrupt dummy end block. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(&d, chunk, 100);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);

	memset(chunk + (100 - DUMMY_END_BLOCK_BAUS) * HMM_BLOCK_ALIGN_UNIT,
	       0xA5, sizeof(thmm_head_record));
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_shrink_chunk(&d, 1);

	WRONG
      }

    /* Corrupt last block. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(&d, chunk, 100);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);

	whack_head(p[0]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_shrink_chunk(&d, 1);

	WRONG
      }

    /* Shrink after alloc not free. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(&d, chunk, 100);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);

	if ((p[0] = thmm_from_heap(thmm_largest_available(&d))) == 0)
	  WRONG
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_shrink_chunk(&d, 1);

	WRONG
      }

    /* Shrink more than chunk size. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(&d, chunk, 100);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_shrink_chunk(&d, 101);

	WRONG
      }

    /* Shrink without leaving enough space for minimal chunk. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(&d, chunk, 100);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_shrink_chunk(&d, 101 - MIN_BLOCK_BAUS - DUMMY_END_BLOCK_BAUS);

	WRONG
      }
  }

void audit_fail_resize_tests(void)
  {
    /* Resize corrupt block. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(
	  &d, chunk, (4 * (MIN_BLOCK_BAUS + 1)) + DUMMY_END_BLOCK_BAUS);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG
	if ((p[1] = thmm_from_heap(0)) == 0)
	  WRONG
	if ((p[2] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
	thmm_to_heap(p[2]);

	whack_head(p[1]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_resize(&d, p[1], MIN_BLOCK_PYLD_AAUS + 1);

	WRONG
      }

    /* Resize free block. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(
	  &d, chunk, (4 * (MIN_BLOCK_BAUS + 1)) + DUMMY_END_BLOCK_BAUS);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG
	if ((p[1] = thmm_from_heap(0)) == 0)
	  WRONG
	if ((p[2] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
	thmm_to_heap(p[2]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_resize(&d, p[2], MIN_BLOCK_PYLD_AAUS + 1);

	WRONG
      }

    /* Resize with corrupt AVL tree root block. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(
	  &d, chunk, (4 * (MIN_BLOCK_BAUS + 1)) + DUMMY_END_BLOCK_BAUS);

	if ((p[0] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS)) == 0)
	  WRONG
	if ((p[1] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS)) == 0)
	  WRONG
	if ((p[2] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS + 1)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
	thmm_to_heap(p[2]);

	whack_head(p[0]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_resize(&d, p[1], MIN_BLOCK_PYLD_AAUS + 1);

	WRONG
      }

    /* Corrupt next free block. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(
	  &d, chunk, (4 * (MIN_BLOCK_BAUS + 1)) + DUMMY_END_BLOCK_BAUS);

	if ((p[0] = thmm_from_heap(0)) == 0)
	  WRONG
	if ((p[1] = thmm_from_heap(0)) == 0)
	  WRONG
	if ((p[2] = thmm_from_heap(0)) == 0)
	  WRONG

	thmm_to_heap(p[0]);
	thmm_to_heap(p[2]);

	((thmm_head_record *)
         (((char *) p[2]) - (HEAD_AAUS * HMM_ADDR_ALIGN_UNIT)))->block_size =
	   666;
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_resize(&d, p[1], MIN_BLOCK_PYLD_AAUS + 1);

	WRONG
      }

    /* Corrupt last block freed. */

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_init(&d);

	thmm_new_chunk(
	  &d, chunk, (4 * (MIN_BLOCK_BAUS + 2)) + DUMMY_END_BLOCK_BAUS);

	if ((p[0] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS)) == 0)
	  WRONG
	if ((p[1] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS + 1)) == 0)
	  WRONG
	if ((p[2] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS)) == 0)
	  WRONG
	if ((p[3] = thmm_from_heap(MIN_BLOCK_PYLD_AAUS + 2)) == 0)
	  WRONG

	thmm_to_heap(p[1]);
	thmm_to_heap(p[3]);
	thmm_to_heap(p[0]);

	whack_head(p[0]);
      }
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_resize(&d, p[2], MIN_BLOCK_PYLD_AAUS + 1);

	WRONG
      }
  }

void audit_fail_tests(void)
  {
    audit_fail_test_prep();

    /* Corrupt the head of the first allocated block, then free it. */

    whack_head(p[1]);

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_to_heap(p[1]);

	/* We'll only get here if the long jump does not occur. */
	WRONG
      }

    audit_fail_test_prep();

    /* Corrupt the head of the second free block, then free the
    ** allocated block that proceeds it.  This should cause the audit
    ** of the allocated block to fail.
    */

    whack_head(p[2]);

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_free(&d, p[1]);

	WRONG
      }

    audit_fail_test_prep();

    /* Corrupt the head of the last free block, then try to allocate from
    ** it.
    */

    whack_head(p[4]);

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_from_heap(14);

	WRONG
      }

    audit_fail_test_prep();

    /* Corrupt the head of the second free block, then try to allocate
    ** from the last free block.  The second free block should be above
    ** the last free block in the AVL tree.
    */

    whack_head(p[2]);

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_from_heap(14);

	WRONG
      }

    audit_fail_test_prep();

    /* Corrupt the head of the second allocated block, then try to allocate
    ** from the last free block.  The second free block should be above
    ** the last free block in the AVL tree. Corrupting the head of the
    ** second allocated block should cause the audit of the (immediately
    ** preceeding) second free block to fail.
    */

    whack_head(p[3]);

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_from_heap(14);

	WRONG
      }

    audit_fail_test_prep();

    /* Corrupt the head of the first free block, then try to allocate
    ** from the last free block.  The first free block should be above
    ** the last free block in the AVL tree.
    */

    whack_head(p[0]);

    if (setjmp(thmm_jmp_buf) == 0)
      {
	thmm_from_heap(14);

	WRONG
      }

    audit_fail_shrink_tests();

    audit_fail_resize_tests();
  }

int main(void)
  {
    /* The do_tests function is defined in test_hmm.h. */

    printf("\nHMM TESTS\n\n");
    hmm_do_tests();

    printf("\nTHMM TESTS\n\n");
    if (setjmp(thmm_jmp_buf) == 0)
      thmm_do_tests();
    else
      {
	printf(
	  "THMM AUDIT FAIL: File=%s Line=%u\n",
	  thmm_fail_file, thmm_fail_line);
	WRONG
      }

    printf("\nAUDIT FAIL TESTS\n\n");
    audit_fail_tests();

    printf("\nSUCCESS!\n");
    return(0);
  }

/* Compile all the implementation files with HMM_CNFG_NUM defined to 0. */

#undef HEAD_AAUS
#undef PTR_RECORD_AAUS

#include "hmm_base.c"
#include "hmm_alloc.c"
#include "hmm_true.c"
#include "hmm_largest.c"
#include "hmm_grow.c"
#include "hmm_shrink.c"
#include "hmm_resize.c"
