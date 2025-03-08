/* Version 1.1
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

/* External header file for Heap Memory Manager.  See documentation in
** heapmm.html.
*/

#undef HMM_PROCESS

/* Include once per configuration in a particular translation unit. */

#ifndef HMM_CNFG_NUM

/* Default configuration. */

#ifndef HMM_INC_CNFG_DFLT
#define HMM_INC_CNFG_DFLT
#define HMM_PROCESS
#endif

#elif HMM_CNFG_NUM == 0

/* Test configuration. */

#ifndef HMM_INC_CNFG_0
#define HMM_INC_CNFG_0
#define HMM_PROCESS
#endif

#elif HMM_CNFG_NUM == 1

#ifndef HMM_INC_CNFG_1
#define HMM_INC_CNFG_1
#define HMM_PROCESS
#endif

#elif HMM_CNFG_NUM == 2

#ifndef HMM_INC_CNFG_2
#define HMM_INC_CNFG_2
#define HMM_PROCESS
#endif

#elif HMM_CNFG_NUM == 3

#ifndef HMM_INC_CNFG_3
#define HMM_INC_CNFG_3
#define HMM_PROCESS
#endif

#elif HMM_CNFG_NUM == 4

#ifndef HMM_INC_CNFG_4
#define HMM_INC_CNFG_4
#define HMM_PROCESS
#endif

#elif HMM_CNFG_NUM == 5

#ifndef HMM_INC_CNFG_5
#define HMM_INC_CNFG_5
#define HMM_PROCESS
#endif

#endif

#ifdef HMM_PROCESS

#include "hmm_cnfg.h"

/* Heap descriptor. */
typedef struct HMM_UNIQUE(structure)
  {
    /* private: */

    /* Pointer to (payload of) root node in AVL tree.  This field should
    ** really be the AVL tree descriptor (type avl_avl).  But (in the
    ** instantiation of the AVL tree generic package used in package) the
    ** AVL tree descriptor simply contains a pointer to the root.  So,
    ** whenever a pointer to the AVL tree descriptor is needed, I use the
    ** cast:
    **
    ** (avl_avl *) &(heap_desc->avl_tree_root)
    **
    ** (where heap_desc is a pointer to a heap descriptor).  This trick
    ** allows me to avoid including cavl_if.h in this external header. */
    void *avl_tree_root;

    /* Pointer to first byte of last block freed, after any coalescing. */
    void *last_freed;

    /* public: */

    HMM_UNIQUE(size_bau) num_baus_can_shrink;
    void *end_of_shrinkable_chunk;
  }
HMM_UNIQUE(descriptor);

/* Prototypes for externally-callable functions. */

void HMM_UNIQUE(init)(HMM_UNIQUE(descriptor) *desc);

void * HMM_UNIQUE(alloc)(
  HMM_UNIQUE(descriptor) *desc, HMM_UNIQUE(size_aau) num_addr_align_units);

/* NOT YET IMPLEMENTED */
void * HMM_UNIQUE(greedy_alloc)(
  HMM_UNIQUE(descriptor) *desc, HMM_UNIQUE(size_aau) needed_addr_align_units,
  HMM_UNIQUE(size_aau) coveted_addr_align_units);

int HMM_UNIQUE(resize)(
  HMM_UNIQUE(descriptor) *desc, void *mem,
  HMM_UNIQUE(size_aau) num_addr_align_units);

/* NOT YET IMPLEMENTED */
int HMM_UNIQUE(greedy_resize)(
  HMM_UNIQUE(descriptor) *desc, void *mem,
  HMM_UNIQUE(size_aau) needed_addr_align_units,
  HMM_UNIQUE(size_aau) coveted_addr_align_units);

void HMM_UNIQUE(free)(HMM_UNIQUE(descriptor) *desc, void *mem);

HMM_UNIQUE(size_aau) HMM_UNIQUE(true_size)(void *mem);

HMM_UNIQUE(size_aau) HMM_UNIQUE(largest_available)(
  HMM_UNIQUE(descriptor) *desc);

void HMM_UNIQUE(new_chunk)(
  HMM_UNIQUE(descriptor) *desc, void *start_of_chunk,
  HMM_UNIQUE(size_bau) num_block_align_units);

void HMM_UNIQUE(grow_chunk)(
  HMM_UNIQUE(descriptor) *desc, void *end_of_chunk,
  HMM_UNIQUE(size_bau) num_block_align_units);

/* NOT YET IMPLEMENTED */
void HMM_UNIQUE(shrink_chunk)(
  HMM_UNIQUE(descriptor) *desc,
  HMM_UNIQUE(size_bau) num_block_align_units);

#endif /* defined HMM_PROCESS */
