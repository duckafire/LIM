#ifndef CORE_TOOLS_QUEUE
#define CORE_TOOLS_QUEUE

#include <stdio.h>
#include <stdbool.h>

typedef struct Queue{
	char *ident;
	char *table_key;
	unsigned short quantity;
	bool is_const;
	struct Queue *next;
}Queue;

Queue* qee_create(char *ident, char *table_key, bool is_const);
bool qee_add_item(Queue **head, char *ident, char *table_key, bool is_const, bool upQtt);
static Queue* insert_item(Queue *item);
static Queue* ordenate_queue(Queue *item);
Queue *qee_get_item(Queue *item, char *ident);
void qee_free_queue(Queue *item);
static void free_item(Queue *item);
void qee_bigger_to_lower(bool allow);

#endif
