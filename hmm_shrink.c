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

#include "hmm_intrnl.h"

void U(shrink_chunk)(U(descriptor) *desc, U(size_bau) n_baus_to_shrink)
  {
    head_record *dummy_end_block = (head_record *)
      BAUS_BACKWARD(desc->end_of_shrinkable_chunk, DUMMY_END_BLOCK_BAUS);

    #ifdef HMM_AUDIT_FAIL

    if (dummy_end_block->block_size != 0)
      /* Chunk does not have valid dummy end block. */
      HMM_AUDIT_FAIL

    #endif

    if (n_baus_to_shrink)
      {
	head_record *last_block = (head_record *)
	  BAUS_BACKWARD(
	    dummy_end_block, dummy_end_block->previous_block_size);

	#ifdef HMM_AUDIT_FAIL
	AUDIT_BLOCK(last_block)
	#endif

	if (last_block == desc->last_freed)
	  {
	    U(size_bau) bs = BLOCK_BAUS(last_block);

	    /* Chunk will not be shrunk out of existence if
	    ** 1.  There is at least one allocated block in the chunk
	    **     and the amount to shrink is exactly the size of the
	    **     last block, OR
	    ** 2.  After the last block is shrunk, there will be enough
	    **     BAUs left in it to form a minimal size block. */
	    int chunk_will_survive =
	      (PREV_BLOCK_BAUS(last_block) && (n_baus_to_shrink == bs)) ||
	      (n_baus_to_shrink <= (U(size_bau)) (bs - MIN_BLOCK_BAUS));

	    if (chunk_will_survive ||
		(!PREV_BLOCK_BAUS(last_block) &&
		 (n_baus_to_shrink ==
		  (U(size_bau)) (bs + DUMMY_END_BLOCK_BAUS))))
	      {
		desc->last_freed = 0;

		if (chunk_will_survive)
		  {
		    bs -= n_baus_to_shrink;
		    if (bs)
		      {
			/* The last (non-dummy) block was not completely
			** eliminated by the shrink. */

			last_block->block_size = bs;

			/* Create new dummy end record.
			*/
			dummy_end_block =
			  (head_record *) BAUS_FORWARD(last_block, bs);
			dummy_end_block->previous_block_size = bs;
			dummy_end_block->block_size = 0;

			#ifdef HMM_AUDIT_FAIL
			if (desc->avl_tree_root)
			  AUDIT_BLOCK(PTR_REC_TO_HEAD(desc->avl_tree_root))
			#endif

			U(into_free_collection)(desc, last_block);
		      }
		    else
		      {
			/* The last (non-dummy) block was completely
			** eliminated by the shrink.  Make its head
			** the new dummy end block.
			*/
			last_block->block_size = 0;
			last_block->previous_block_size &= ~HIGH_BIT_BAU_SIZE;
		      }
		  }
	      }
	    #ifdef HMM_AUDIT_FAIL
	    else
	      HMM_AUDIT_FAIL
	    #endif
	  }
	#ifdef HMM_AUDIT_FAIL
	else
	  HMM_AUDIT_FAIL
	#endif
      }
  }
