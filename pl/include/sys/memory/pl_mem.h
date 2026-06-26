#pragma once

#include "sys/pl_types.h"

static const usize PL_MEM_MAX_ALIGN = alignof(max_align_t);

static usize pl_mem_align_up(usize value) {
  return (value + PL_MEM_MAX_ALIGN - 1) & ~(PL_MEM_MAX_ALIGN - 1);
}

// Live-allocation diagnostics, read via pl_mem_get_stats().
typedef struct {
  usize block_count; // live blocks: pl_mem_malloc calls not yet matched by pl_mem_free
  usize bytes;       // sum of the requested sizes of those live blocks
} pl_mem_stats;

// malloc/free wrappers that keep the diagnostics above. Memory comes back
// uninitialized (malloc semantics) - the caller owns init.
void *pl_mem_malloc(usize size);
// Frees the block and sets *ptr to nullptr, so calling it again is a safe
// no-op. *ptr must have come from pl_mem_malloc.
void pl_mem_free(void **ptr);
pl_mem_stats pl_mem_get_stats(void);
