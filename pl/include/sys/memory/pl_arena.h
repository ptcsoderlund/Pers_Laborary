#pragma once

#include "sys/pl_error.h"
#include "sys/pl_types.h"

typedef struct pl_arena pl_arena;

pl_error pl_arena_new(pl_arena** out_arena, usize capacity);

pl_error pl_arena_alloc(pl_arena* arena, usize size, void** out_ptr);
pl_error pl_arena_reset(pl_arena* arena);
//Zero the used region (bytes allocated so far). Call before reset().
pl_error pl_arena_zero(pl_arena* arena);

usize pl_arena_free_space_left(pl_arena* arena);

pl_error pl_arena_free(pl_arena** arena);
