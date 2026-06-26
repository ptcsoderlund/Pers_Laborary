#pragma once

#include "sys/pl_types.h"
#include <assert.h>
#include <string.h>

enum pl_error_code {
  PL_ERROR_NONE = 0,
  PL_ERROR_NULL_PARAM = 1,
  PL_ERROR_OUT_OF_MEMORY = 2,
  PL_ERROR_BAD_REQUEST = 3,
  PL_ERROR_OUT_OF_BOUNDS = 4
};

typedef struct {
  const char *message;
  enum pl_error_code code;
} pl_error;

static inline pl_error pl_error_none(void) {
  return (pl_error){.code = PL_ERROR_NONE, .message = ""};
}
//Runtime checks
static inline bool pl_error_is_none(pl_error err){
  return err.code == PL_ERROR_NONE;
}

static inline pl_error pl_error_first_or_none(const pl_error errors[],
                                                    usize errors_count) {
  if (errors == nullptr)
    return pl_error_none();
  for (usize i = 0; i < errors_count; i++) {
    pl_error current = errors[i];
    if (current.code != PL_ERROR_NONE) {
      return current;
    }
  }
  return pl_error_none();
}


static inline pl_error pl_error_if(bool is_error,enum pl_error_code error_code,const char* error_message){
  if(!is_error)
    return pl_error_none();
  return (pl_error){ .code = error_code,.message=error_message};
}

//Dev time checks
static inline void pl_error_assert(pl_error err){
  assert(err.code == PL_ERROR_NONE);
  assert(err.message != nullptr);
  assert(strcmp(err.message,"") == 0);
}

#define pl_error_first_or_none_x(error_arr) \
  pl_error_first_or_none((error_arr),(sizeof(error_arr)/sizeof((error_arr)[0])))
