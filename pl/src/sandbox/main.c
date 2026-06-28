#include <sys/collections/pl_darray.h>
#include <sys/memory/pl_arena.h>
#include <sys/memory/pl_mem.h>
#include <sys/pl_error.h>
#include <sys/pl_print.h>
#include <sys/pl_types.h>

void arena_test(){
  pl_print_info_ln("------ arena_test ------");
  u8 error_count = 0;
  pl_arena* arena = nullptr;
  const usize mem_size = 1024;
  pl_arena_new(&arena,mem_size);
  i32* n1 = nullptr;
  i32* n2 = nullptr;
  i32* n3 = nullptr;
  pl_arena_alloc(arena,sizeof(i32),(void*)&n1);
  pl_arena_alloc(arena,sizeof(i32),(void*)&n2);
  pl_arena_alloc(arena,sizeof(i32),(void*)&n3);
  *n1 = 200;
  *n2 = 300;
  *n3 = 400;
  usize expected_size = mem_size - pl_mem_align_up(sizeof(i32))*3;
  if(expected_size != pl_arena_free_space_left(arena))
  {pl_print_error_ln("Expected size of arena mismatch, expecting: %zu, got: %zu",expected_size,pl_arena_free_space_left(arena));
    error_count++;
  }
  pl_arena_zero(arena);
  if(*n1 != 0 || *n2 != 0 || *n3 != 0)
  {pl_print_error_ln("Failed to zero out arena memory");
    error_count++;
  }
  if(error_count == 0)
    pl_print_info_ln("Arena test completed successfully");
  pl_arena_reset(arena);
  if(pl_arena_free_space_left(arena) != mem_size){
    pl_print_error_ln("Could not reset arena");
    error_count++;
  }
  pl_arena_free(&arena);
}
void darray_test(){
  pl_darray* my_arr = nullptr;
  pl_darray_new(&my_arr,sizeof(i32));
  i32 add_me = 200;
  pl_darray_add(&my_arr,&add_me);
  add_me = 400;
  pl_darray_add(&my_arr,&add_me);
  i32* item = nullptr;
  usize index = 0;
  while(pl_darray_for_i(my_arr,(void**)&item,&index)){
    pl_print_info_ln("darray_for_i, index: %u,item: %i",index,*item);
  }
  pl_darray_free(&my_arr);
}

int main(void) {
  pl_print_info_ln("main");
  arena_test();
  darray_test();
  return 0;
}
