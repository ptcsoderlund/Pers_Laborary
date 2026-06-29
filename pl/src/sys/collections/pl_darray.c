#include "sys/collections/pl_darray.h"
#include "sys/memory/pl_mem.h"
#include "sys/pl_error.h"
#include "sys/pl_types.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pl_darray_header {
  usize byte_capacity;
  usize bytes_per_item;
  usize item_count;
} pl_darray_header;

static usize header_size() { return pl_mem_align_up(sizeof(pl_darray_header)); }

pl_error pl_darray_new(pl_darray **arr_out, usize item_size) {
  // Dev-time: trip the moment a contract is broken.
  assert(arr_out != nullptr);
  assert(item_size >= 1);
  // Runtime: same contract, recover instead of crashing in release builds.
  pl_error error_checks[] = {
      pl_error_if(arr_out == nullptr, PL_ERROR_NULL_PARAM,
                     "arr_out is null"),
      pl_error_if(item_size < 1, PL_ERROR_BAD_REQUEST,
                     "item_size cannot be less then 1"),
  };
  pl_error error_checks_result = pl_error_first_or_none_x(error_checks);
  if (error_checks_result.code != PL_ERROR_NONE)
    return error_checks_result;

  usize initial_capacity = PL_DARRAY_INITIAL_CAPACITY;
  while (initial_capacity < item_size)
    initial_capacity *= 2;
  u8 *block = malloc(header_size() + initial_capacity);
  if (!block)
    return (pl_error){.code = PL_ERROR_OUT_OF_MEMORY,
                         .message = "could not create dynamic array"};
  pl_darray_header *header = (pl_darray_header *)block;
  header->byte_capacity = initial_capacity;
  header->bytes_per_item = item_size;
  header->item_count = 0;
  // The handle points at the header; the payload lives right after it.
  (*arr_out) = (pl_darray *)block;
  return pl_error_none();
}

pl_error pl_darray_add(pl_darray **arr, const void *item) {
  assert(arr != nullptr);
  assert(*arr != nullptr);
  assert(item != nullptr);
  pl_error error_checks[] = {
      pl_error_if(arr == nullptr, PL_ERROR_NULL_PARAM, "arr is null"),
      pl_error_if(arr != nullptr && *arr == nullptr, PL_ERROR_NULL_PARAM,
                     "*arr is null"),
      pl_error_if(item == nullptr, PL_ERROR_NULL_PARAM, "item is null"),
  };
  pl_error error_checks_result = pl_error_first_or_none_x(error_checks);
  if (error_checks_result.code != PL_ERROR_NONE)
    return error_checks_result;

  pl_darray_header *header = (pl_darray_header *)(*arr);
  usize used = header->item_count * header->bytes_per_item;
  if (used + header->bytes_per_item > header->byte_capacity) {
    usize new_capacity = header->byte_capacity;
    while (used + header->bytes_per_item > new_capacity)
      new_capacity *= 2;
    // realloc may move the block, so write the new handle back through `arr`.
    u8 *block = realloc(*arr, header_size() + new_capacity);
    if (!block)
      return (pl_error){.code = PL_ERROR_OUT_OF_MEMORY,
                           .message = "could not grow dynamic array"};
    *arr = (pl_darray *)block;
    header = (pl_darray_header *)block;
    header->byte_capacity = new_capacity;
  }

  u8 *payload = (u8 *)(*arr) + header_size();
  memcpy(payload + used, item, header->bytes_per_item);
  header->item_count += 1;
  return pl_error_none();
}

pl_error pl_darray_get_item_val_at(pl_darray *arr, usize index, void *result) {
  assert(arr != nullptr);
  assert(result != nullptr);
  pl_error param_checks[] = {
      pl_error_if(arr == nullptr, PL_ERROR_NULL_PARAM, "arr is null"),
      pl_error_if(result == nullptr, PL_ERROR_NULL_PARAM,
                     "result is null"),
  };
  pl_error param_checks_result = pl_error_first_or_none_x(param_checks);
  if (!pl_error_is_none(param_checks_result))
    return param_checks_result;
  pl_darray_header *header = (pl_darray_header *)arr;
  assert(index < header->item_count);
  pl_error error_check =
      pl_error_if(header->item_count <= index, PL_ERROR_OUT_OF_BOUNDS,
                     "index out of range");
  if (!pl_error_is_none(error_check))
    return error_check;
  u8 *payload_start = (u8 *)arr + header_size();
  u8 *item_start = payload_start + index * header->bytes_per_item;
  memcpy(result,item_start,header->bytes_per_item);
  return pl_error_none();
}
pl_error pl_darray_set_item_val_at(pl_darray* arr,usize index,const void* value){
  assert(arr != nullptr);
  assert(value != nullptr);
  pl_error param_checks[] = {
      pl_error_if(arr == nullptr, PL_ERROR_NULL_PARAM, "arr is null"),
      pl_error_if(value == nullptr, PL_ERROR_NULL_PARAM,
                     "value is null"),
  };
  pl_error param_checks_result = pl_error_first_or_none_x(param_checks);
  if (!pl_error_is_none(param_checks_result))
    return param_checks_result;
  pl_darray_header *header = (pl_darray_header *)arr;
  assert(index < header->item_count);
  pl_error error_check =
      pl_error_if(header->item_count <= index, PL_ERROR_OUT_OF_BOUNDS,
                     "index out of range");
  if (!pl_error_is_none(error_check))
    return error_check;
  u8 *payload_start = (u8 *)arr + header_size();
  u8 *item_start = payload_start + index * header->bytes_per_item;
  memcpy(item_start,value,header->bytes_per_item);
  return pl_error_none();
}

bool pl_darray_foreach(pl_darray* arr, void** item_iter){
  assert(arr != nullptr);
  assert(item_iter != nullptr);
  pl_darray_header* header = (pl_darray_header*)arr;
  u8* payload_start = (u8*)arr+header_size();
  u8* next_item = (*item_iter) == nullptr ? payload_start : (u8*)(*item_iter)+header->bytes_per_item;
  usize dist_next = next_item-payload_start;
  if(dist_next >= header->item_count*header->bytes_per_item){
    *item_iter = nullptr;
    return false;
  }
  *item_iter = next_item;
  return true;
}

bool pl_darray_for_i(pl_darray *arr, void **item_iter, usize *index_out) {
  assert(index_out != nullptr);
  assert(item_iter != nullptr);
  if (*item_iter == nullptr) {
    *index_out = SIZE_MAX;//we increment into first element with overflow
  }
  if (pl_darray_foreach(arr, item_iter)) {
    *index_out += 1;
    return true;
  }
  return false;
}

pl_error pl_darray_remove(pl_darray *arr, usize index) {
  assert(arr != nullptr);
  pl_error null_check =
      pl_error_if(arr == nullptr, PL_ERROR_NULL_PARAM, "arr is null");
  if (!pl_error_is_none(null_check))
    return null_check;
  pl_darray_header *header = (pl_darray_header *)arr;
  assert(index < header->item_count);
  pl_error error_checks[] = {
      pl_error_if(header->item_count <= index, PL_ERROR_OUT_OF_BOUNDS,
                     "Index out of bounds"),
      pl_error_if(header->item_count < 1, PL_ERROR_OUT_OF_BOUNDS,
                     "dynamic array is empty")};
  pl_error error_checks_result = pl_error_first_or_none_x(error_checks);
  if (!pl_error_is_none(error_checks_result))
    return error_checks_result;
  u8 *index_ptr = (u8 *)arr + header_size() + (header->bytes_per_item * index);
  u8 *index_plus_one = index_ptr + header->bytes_per_item;
  header->item_count--;
  memmove(index_ptr, index_plus_one,
          (header->item_count - index) * header->bytes_per_item);
  return pl_error_none();
}

pl_error pl_darray_clear(pl_darray *arr, bool set_to_zero) {
  assert(arr != nullptr);
  if (arr == nullptr)
    return (pl_error){.code = PL_ERROR_NULL_PARAM,
                         .message = "arr is null"};
  pl_darray_header *header = (pl_darray_header *)arr;
  u8 *payload = (u8 *)header + header_size();
  if (set_to_zero)
    memset(payload, 0, header->item_count * header->bytes_per_item);
  header->item_count = 0;
  return pl_error_none();
}
usize pl_darray_count_items(pl_darray* arr){
  assert(arr != nullptr);
  pl_darray_header* header = (pl_darray_header*)arr;
  return header->item_count;
}
usize pl_darray_byte_capacity(pl_darray* arr){
  assert(arr != nullptr);
  pl_darray_header* header = (pl_darray_header*)arr;
  return header->byte_capacity;
}

pl_error pl_darray_free(pl_darray** arr){
  assert(arr != nullptr);
  if(arr == nullptr){
    return (pl_error){.code=PL_ERROR_NULL_PARAM,.message="dynamic array is null"};
  }
  free((*arr));
  (*arr) = nullptr;
  return pl_error_none();
}
