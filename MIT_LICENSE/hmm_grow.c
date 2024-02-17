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

void U(grow_chunk)(U(descriptor) *desc, void *end, U(size_bau) n_baus)
  {
    #undef HEAD_PTR
    #define HEAD_PTR ((head_record *) end)

    end = BAUS_BACKWARD(end, DUMMY_END_BLOCK_BAUS);

    #ifdef HMM_AUDIT_FAIL

    if (HEAD_PTR->block_size != 0)
      /* Chunk does not have valid dummy end block. */
      HMM_AUDIT_FAIL

    #endif

    /* Create a new block that absorbs the old dummy end block. */
    HEAD_PTR->block_size = n_baus;

    /* Set up the new dummy end block. */
    {
      head_record *dummy = (head_record *) BAUS_FORWARD(end, n_baus);
      dummy->previous_block_size = n_baus;
      dummy->block_size = 0;
    }

    /* Simply free the new block, allowing it to coalesce with any
    ** free block at that was the last block in the chunk prior to
    ** growth.
    */
    U(free)(desc, HEAD_TO_PTR_REC(end));

    #undef HEAD_PTR
  }
