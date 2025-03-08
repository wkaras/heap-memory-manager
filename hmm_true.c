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

U(size_aau) U(true_size)(void *payload_ptr)
  {
    register  head_record *head_ptr = PTR_REC_TO_HEAD(payload_ptr);

    #ifdef HMM_AUDIT_FAIL
    AUDIT_BLOCK(head_ptr)
    #endif

    /* Convert block size from BAUs to AAUs.  Subtract head size, leaving
    ** payload size.
    */
    return(
      (BLOCK_BAUS(head_ptr) * ((U(size_aau)) HMM_BLOCK_ALIGN_UNIT)) -
      HEAD_AAUS);
  }
