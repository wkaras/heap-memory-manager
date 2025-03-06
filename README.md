# heap-memory-manager
Heap memory manager in C (not thread safe) -- uses https://wkaras.github.io/gen_c/cavl_tree.html

Follow the link https://wkaras.github.io/heapmm/heapmm.html to easily read the HTML documentation in this repository.

This is not thread-safe. It wrote it back in 2002, when single-core CPUs were still common.  I think it's a bit more
straightforward to read the code than many allocators.

This code is currently provided with an MIT license.  The subdirectory PUBLIC_DOMAIN contains the code assigned to the public
domain.  I'm not planning to actively maintain/update the public domain code.  Apparently, there is some legal controversy as to
whether authors can put their own work into the public domain.
