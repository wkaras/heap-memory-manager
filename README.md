# heap-memory-manager
Heap memory manager in C (not thread safe) -- uses https://wkaras.github.io/gen_c/cavl_tree.html

Follow the link https://wkaras.github.io/heapmm/heapmm.html to easily read the HTML documentation in this repository.

This is not thread-safe. It wrote it back in 2002, when single-core CPUs were still common.  I think it's a bit more
straightforward to read the code than many allocators.

The MIT_LICENSE subdirectory contains copies of all the source and documentation files.  The only difference is that they
contain an MIT license, rather than an assignment to the public domain.  Apparently, there is some legal controversy as
to whether authors can put their own work into the public domain.
