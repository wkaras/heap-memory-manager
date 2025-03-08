#!/bin/env bash

CC="clang --pedantic -Wall -Wextra -Wno-uninitialized -g"

$CC test_hmm.c hmm_alloc.c hmm_base.c hmm_true.c hmm_resize.c hmm_largest.c hmm_shrink.c hmm_dflt_abort.c hmm_grow.c -o test_hmm
