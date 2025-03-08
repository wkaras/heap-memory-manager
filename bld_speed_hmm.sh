#!/bin/env bash

CC="clang --pedantic -Wall -Wextra -Wno-uninitialized -O3"

$CC speed_hmm.c hmm_alloc.c hmm_base.c hmm_dflt_abort.c -o speed_hmm
