#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queuePointer.h"

static Queue firstItem = {false, NULL, NULL};
Queue nullItem = {false, NULL, NULL};

Queue* newItemToQueue(char *value, bool dinamicValue){
	static Queue *lastItem = &firstItem;

	size_t size = sizeof(Queue);
	Queue *new = malloc(size);
	memset(new, '\0', size);

	lastItem->next = new;

	new->dinamicValue = dinamicValue;
	new->value = value;
	new->next  = NULL;

	lastItem = new;
	return lastItem;
}

void clearQueue(void){
	Queue *p, *n;
	for(p = firstItem.next; p != NULL; p = n){
		n = p->next;
		if(p->dinamicValue) free(p->value);
		free(p);
	}
}
