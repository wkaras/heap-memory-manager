# heap-memory-manager
Heap memory manager in C (not thread safe) -- uses https://wkaras.github.io/gen_c/cavl_tree.html

Follow the link https://wkaras.github.io/heapmm/heapmm.html to easily read the HTML documentation in this repository.

This is not thread-safe. It wrote it back in 2002, when single-core CPUs were still common.  I think it's a bit more
straightforward to read the code than many allocators.

This code is currently provided with an MIT license.  The subdirectory PUBLIC_DOMAIN contains the code assigned to the public
domain.  I'm not planning to actively maintain/update the public domain code.  Apparently, there is some legal controversy as to
whether authors can put their own work into the public domain.

One important thing I neglected to mention/realize is that the audit tests can result in a read of a bad address, which in some environments will cause an internal interrupt.  If you see this, it's not a reason to be concerned that heapmm won't work in your environment.

To reduce memory space overhead, this allocator uses the memory within free blocks to create a searchable data structure (an AVL tree) of  free blocks.  The potential big trade-off with that is it will result in a lot of memory accesses with terrible locality.  That is, a low ratio of number of memory accesses to number of memory pages containing the accessed addresses.  This could be a significant problem for an application that uses a large amount of dynamically allocated memory, and runs in an environment with insufficient physical memory that it can run without page faults.  Lack of locality can also have an effect even if memory pages are not paged out to and back from secondary storage.  On higher-end Freescale processors, for example, the Memory Management Unit (MMU) can be useful even if there is no actual paging of memory.  But the MMU uses a large table in physical memory that  defines the mapping of virtual memory pages to physical memory pages.  The MMU is only able to cache a limitied number of entries in this table.  So an access to a memory page whose translation table entry is not cached will cause two memory accesses, one to load in the translation entry, followd by the actual access itself.  So this case, bad locality of memory accesses can have a price even without memory paging.

This allocator, as is, is obsolete now because it is not multithread-safe. This could perhaps be used for per-thread memory polls. If memory was deallocated in a different thread than it was allocated, it could be put in a linked list associated with the allocating code. When the allocation function is called in a thread, it could pop all the blocks that had been put in its associated linked list, and put them into its memory pool, before doing the allocation. The head pointers for the free lists would have to be atomic, and versioned due to the ABA problem.
