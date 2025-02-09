#ifndef CORE_PROCESS_MASTER
#define CORE_PROCESS_MASTER

void read_source_file(void);
static void get_and_put_from_buffer(FILE *buf, bool separator);
void build_destine_file(void);

#endif
