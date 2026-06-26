#pragma once
#include "sys/pl_error.h"
typedef struct pl_darray pl_darray;

static const usize PL_DARRAY_INITIAL_CAPACITY = 256;
pl_error pl_darray_new(pl_darray** arr_out,usize item_size);
pl_error pl_darray_add(pl_darray** arr,const void* item);
pl_error pl_darray_get_item_at(pl_darray* arr,usize index,void** result);
//Use this in a while loop.
//When passing in current_item as nullptr we start iterating from beginning.
bool pl_darray_foreach(pl_darray* arr,void** current_item);
bool pl_darray_for_i(pl_darray* arr,void** current_item,usize* index_out);
pl_error pl_darray_remove(pl_darray* arr,usize index);
//Clear memory but keep capacity
pl_error pl_darray_clear(pl_darray* arr,bool set_to_zero);
usize pl_darray_count_items(pl_darray* arr);
usize pl_darray_byte_capacity(pl_darray* arr);
pl_error pl_darray_free(pl_darray** arr);
