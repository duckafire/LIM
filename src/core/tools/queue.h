#ifndef CORE_TOOLS_QUEUE
#define CORE_TOOLS_QUEUE

#include <stdio.h>
#include <stdbool.h>

typedef struct Queue{
	char *content[2];
	unsigned short quantity;
	struct Queue *next;
}Queue;

Queue* qee_create(char *content0, char *content1);
bool qee_add_item(Queue **head, char *content0, char *content1, bool upQtt);
static Queue* insert_item(Queue *item);
static Queue* ordenate_queue(Queue *item);
Queue *qee_get_item(Queue *item, char *content1);
void qee_free_queue(Queue *item);
static void free_item(Queue *item);
void qee_bigger_to_lower(bool allow);

#endif
