# heap-memory-manager
Heap memory manager in C (not thread safe) -- uses https://wkaras.webs.com/gen_c/cavl_tree.html

Follow the link https://wkaras.webs.com/heapmm/heapmm.html to easily read the HTML documentation in this repository.

This is not thread-safe. It wrote it back in 2002, when single-core CPUs were still common.  I think it's a bit more
straightforward to read the code than many allocators.  For some multi-threaded applications, you could use this allocator
(with wrapper functions that take a mutex), and also use https://github.com/ywkaras/MiscRepo/tree/master/Dynamic_obj .
