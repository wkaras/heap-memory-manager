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

void * U(alloc)(U(descriptor) *desc, U(size_aau) n)
  {
    #ifdef HMM_AUDIT_FAIL
    if (desc->avl_tree_root)
      AUDIT_BLOCK(PTR_REC_TO_HEAD(desc->avl_tree_root))
    #endif

    if (desc->last_freed)
      {
	#ifdef HMM_AUDIT_FAIL
	AUDIT_BLOCK(desc->last_freed)
	#endif

	U(into_free_collection)(desc, (head_record *) (desc->last_freed));

	desc->last_freed = 0;
      }

    /* Add space for block header. */
    n += HEAD_AAUS;

    /* Convert n from number of address alignment units to block alignment
    ** units. */
    n = DIV_ROUND_UP(n, HMM_BLOCK_ALIGN_UNIT);

    if (n < MIN_BLOCK_BAUS)
      n = MIN_BLOCK_BAUS;

    {
      /* Search for the first node of the bin containing the smallest
      ** block big enough to satisfy request. */
      ptr_record *ptr_rec_ptr =
	U(avl_search)(
	  (U(avl_avl) *) &(desc->avl_tree_root), (U(size_bau)) n,
	  AVL_GREATER_EQUAL);

      /* If an approprate bin is found, satisfy the allocation request,
      ** otherwise return null pointer. */
      return(ptr_rec_ptr ?
	       U(alloc_from_bin)(desc, ptr_rec_ptr, (U(size_bau)) n) : 0);
    }
  }
