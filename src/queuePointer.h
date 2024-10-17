#ifndef __QUEUE__
#define __QUEUE__

#define NULL_QUEUE_ITEM &nullItem

#include <stdbool.h>

struct Queue {
	bool dinamicValue;
	char *value;
	struct Queue *next;
};

typedef struct Queue Queue;

extern Queue nullItem;

Queue* newItemToQueue(char *value, bool dinamicValue);
void clearQueue(void);

#endif
