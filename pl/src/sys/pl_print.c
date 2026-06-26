#include <stdarg.h>
#include <stdio.h>

#include "sys/pl_print.h"

#define PL_ANSI_RESET_X  "\033[0m"
#define PL_ANSI_CYAN_X   "\033[36m"
#define PL_ANSI_YELLOW_X "\033[33m"
#define PL_ANSI_RED_X    "\033[31m"

void pl_print_info_ln(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, PL_ANSI_CYAN_X "[I]" PL_ANSI_RESET_X " ");
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    va_end(args);
}

void pl_print_warning_ln(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, PL_ANSI_YELLOW_X "[W]" PL_ANSI_RESET_X " ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void pl_print_error_ln(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, PL_ANSI_RED_X "[E]" " ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n" PL_ANSI_RESET_X);
    va_end(args);
}
