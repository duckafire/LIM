#ifndef CORE_COMPACTION_PROCESS_PROCESS_MASTER
#define CORE_COMPACTION_PROCESS_PROCESS_MASTER

void read_source_file(void);
static void get_and_put_from_buffer(FILE *buf, bool separator);
static void build_destine_file(void);

#endif
