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

U(size_aau) U(largest_available)(U(descriptor) *desc)
  {
    U(size_bau) largest;

    if (!(desc->avl_tree_root))
      largest = 0;
    else
      {
	#ifdef HMM_AUDIT_FAIL
	/* Audit root block in AVL tree. */
	AUDIT_BLOCK(PTR_REC_TO_HEAD(desc->avl_tree_root))
	#endif

	largest =
	  BLOCK_BAUS(
	    PTR_REC_TO_HEAD(
	      U(avl_search)(
		(U(avl_avl) *) &(desc->avl_tree_root),
		(U(size_bau)) ~ (U(size_bau)) 0, AVL_LESS)));
      }

    if (desc->last_freed)
      {
	/* Size of last freed block. */
	register U(size_bau) lf_size;

	#ifdef HMM_AUDIT_FAIL
	AUDIT_BLOCK(desc->last_freed)
	#endif

	lf_size = BLOCK_BAUS(desc->last_freed);

	if (lf_size > largest)
	  largest = lf_size;
      }

    /* Convert largest size to AAUs and subract head size leaving payload
    ** size.
    */
    return(largest ?
	     ((largest * ((U(size_aau)) HMM_BLOCK_ALIGN_UNIT)) - HEAD_AAUS) :
	     0);
  }
