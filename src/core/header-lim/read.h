#ifndef CORE_HEADER_FILE_READ
#define CORE_HEADER_FILE_READ

#include <stdio.h>
#include <stdbool.h>
#include "../buf-man/buf-man.h"
#include "out-status.h"

HF_OUT_STATUS read_header_file(char **indiv_part_status);
static bool check_partitions_separator(void);
static bool clear_white_spaces(char h, FILE *dest);
static void start_reading(HF_READ_STATUS *status);
static HF_READ_STATUS read_top_header(void);
static HF_READ_STATUS read_code_scope(void);
static HF_READ_STATUS read_list(Queue **buf);

#endif
