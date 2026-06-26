#include "sys/memory/pl_mem.h"

#include <stdlib.h>

static pl_mem_stats g_stats = {0};

typedef struct {
  usize size;
} pl_mem_header;

static usize header_size(void) { return pl_mem_align_up(sizeof(pl_mem_header)); }

void *pl_mem_malloc(usize size) {
  u8 *block = malloc(header_size() + size);
  if (block == nullptr)
    return nullptr;
  pl_mem_header *header = (pl_mem_header *)block;
  header->size = size;
  g_stats.block_count += 1;
  g_stats.bytes += size;
  return block + header_size();
}

void pl_mem_free(void **ptr) {
  if (ptr == nullptr || *ptr == nullptr)
    return;
  u8 *block = (u8 *)(*ptr) - header_size();
  pl_mem_header *header = (pl_mem_header *)block;
  g_stats.block_count -= 1;
  g_stats.bytes -= header->size;
  free(block);
  *ptr = nullptr;
}

pl_mem_stats pl_mem_get_stats(void) { return g_stats; }
