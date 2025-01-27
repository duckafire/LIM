#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "../debug-common.h"
#include "../../../src/core/tools/queue.h"


static void pitem(Queue *item);
static void pqueue(Queue *item);

static Queue *head;
static char ident_char[7] = "ABCDEF";

#define PARAGRAPH puts("\n\n")
#define PACK(t)   WARN(t); pqueue(head); PARAGRAPH
#define RANIDENTC strchr( ident_char, ident_char[ rand() % 6 ] )

int main(int argc, char *argv[]){
	CHECK_PROGRAM_ARG_QUANTITY

	// big-low
	bool bl_on  = (strcmp(argv[1], "on")  == 0);
	bool bl_off = (strcmp(argv[1], "off") == 0);

	if(!bl_on && !bl_off){
		INVALID_ARG_TO_MAIN(" on | off");

	}else if(bl_on){
		// true by default
		WARN("Organization \"bigger to lower\": ON");

	}else{
		qee_bigger_to_lower(false);
		WARN("Organization \"bigger to lower\": OFF");
	}

	srand(time(NULL));


	head = qee_create(RANIDENTC, NULL, NULL, false);
	PACK("Create a queue with one item");


	char ident[2] = "A";
	short i, j, max;

	for(i = 0; i < 4; i++)
		qee_add_item(&head, RANIDENTC, NULL, NULL, false, QEE_DROP);
	PACK("Add new items to queue (update: off)");


	for(i = 0; i < 4; i++){
		max = 1 + rand() % 5;

		for(j = 0; j < max; j++)
			qee_add_item(&head, RANIDENTC, NULL, NULL, false, QEE_UP_QUANT);
	}
	PACK("Add new items to queue (update: on)");


	WARN("Get 5-9 random items");
	Queue *buf;
	max = rand() % 5 + 5;

	for(i = 0; i < max; i++){
		while( (buf = qee_get_item(head, RANIDENTC)) == NULL );
		pitem(buf);
	}
	PARAGRAPH;


	qee_free_queue(head);
	return 0;
}

static void pitem(Queue *item){
	if(item == NULL)
		return;

	printf("\"This\" (Queue):  %p\n",  item);
	printf("Identifier:     \"%s\"\n", item->ident);
	printf("Table key:      \"%s\"\n", item->table_key);
	printf("Nickname:       \"%s\"\n", item->nick);
	printf("Is constant(?):  %d\n",    item->is_const);
	printf("Quantity:        %d\n",    item->quantity);
	printf("Next (Queue):    %p\n",    item->next);

	putchar('\n');
}

static void pqueue(Queue *item){
	if(item == NULL)
		return;

	pitem(item);

	pqueue(item->next);
}
