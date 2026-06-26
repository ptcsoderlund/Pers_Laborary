#include <stdlib.h>
#include <string.h>

#include "sys/memory/pl_arena.h"
#include "sys/memory/pl_mem.h"
#include "sys/pl_error.h"
#include "sys/pl_types.h"

typedef struct pl_arena_header {
  usize size;
  usize capacity;
  // void* buffer comes after header (or its not a header)
} pl_arena_header;


static usize header_size() { return pl_mem_align_up(sizeof(pl_arena_header)); }

pl_error pl_arena_new(pl_arena **out_arena,
                                  usize capacity) {
  assert(out_arena != nullptr);
  assert(capacity > 0);
  // variable init and error check guards
  u8 *memory = capacity > 0 ? malloc(header_size() + capacity) : nullptr;
  pl_error error_checks[] = {
      pl_error_if(out_arena == nullptr, PL_ERROR_NULL_PARAM,
                         "out_arena is null"),
      pl_error_if(capacity == 0, PL_ERROR_BAD_REQUEST,
                         "Capacity must be greater then 0"),
      pl_error_if(memory == nullptr, PL_ERROR_OUT_OF_MEMORY,
                         "Failed to allocate memory")};
  pl_error error_checks_result = pl_error_first_or_none_x(error_checks);
  if (error_checks_result.code != PL_ERROR_NONE) {
    //nullptr works on free
    free(memory);
    return error_checks_result;
  }
  // yay
  pl_arena_header *header = (pl_arena_header *)memory;
  header->size = 0;
  header->capacity = capacity;
  // The handle points at the header; the payload lives right after it.
  (*out_arena) = (pl_arena *)memory;

  return pl_error_none();
}

pl_error pl_arena_alloc(pl_arena *arena, usize size,
                          void **out_ptr) {
  assert(arena != nullptr);
  assert(size > 0);
  assert(out_ptr != nullptr);
  pl_error error_checks[] = {
    pl_error_if(arena == nullptr,PL_ERROR_NULL_PARAM,"Arena allocator is null"),
    pl_error_if(size == 0,PL_ERROR_BAD_REQUEST,"size must be greater than 0"),
    pl_error_if(out_ptr == nullptr,PL_ERROR_NULL_PARAM,"out_ptr is null"),
  };
  pl_error error_checks_result = pl_error_first_or_none_x(error_checks);
  if(error_checks_result.code != PL_ERROR_NONE)
    return error_checks_result;
  pl_arena_header *header = (pl_arena_header *)arena;
  // Align the running offset so every returned pointer is aligned, regardless
  // of the sizes requested so far.
  if (header->size + size > header->capacity)
    return (pl_error){
        .message = "New allocation in arena would allocate over capacity",
        .code = PL_ERROR_OUT_OF_MEMORY};
  u8* buffer = (u8*)arena + header_size();
  u8* eob = buffer + header->size;
  (*out_ptr) = (u8*)eob;
  header->size += pl_mem_align_up(size);
  return pl_error_none();
}

pl_error pl_arena_reset(pl_arena* arena){
  assert(arena != nullptr);
  if(arena == nullptr)
    return (pl_error){.code=PL_ERROR_NULL_PARAM,.message="arena allocator is null"};
  pl_arena_header* header = (pl_arena_header*)arena;
  header->size = 0;
  return pl_error_none();
}

pl_error pl_arena_zero(pl_arena* arena){
  assert(arena != nullptr);
  if(arena == nullptr)
    return (pl_error){.code=PL_ERROR_NULL_PARAM,.message="arena is null"};
  pl_arena_header* header = (pl_arena_header*)arena;
  u8* buffer_start = (u8*)arena+header_size();
  memset(buffer_start,0,header->size);
  return pl_error_none();
}

usize pl_arena_free_space_left(pl_arena* arena){
  assert(arena != nullptr);
  if(arena == nullptr)
    return 0;
  pl_arena_header* header = (pl_arena_header*)arena;
  return header->capacity - header->size;
}

pl_error pl_arena_free(pl_arena **arena) {
  // Only the handle-to-handle is a hard precondition; an already-null *arena is
  // a tolerated runtime case (idempotent free), so it stays an error return.
  assert(arena != nullptr);
  if (arena == NULL || *arena == NULL)
    return (pl_error){.code = PL_ERROR_NULL_PARAM,
                         .message = "arena allocator is already null"};
  free(*arena);
  *arena = NULL;
  return pl_error_none();
}
