#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../debug-common.h"
#include "../../../src/core/tools/queue.h"


static void pitem(Queue *item);
static void pqueue(Queue *item);

static Queue *head;

#define CONTENT(b) NULL,b
#define PARAGRAPH puts("\n\n")
#define RAND_CTT content[0] = 'a' + rand() % 4
#define PACK(t)   \
	WARN(t);      \
	pqueue(head); \
	PARAGRAPH

int main(int argc, char *argv[]){
	srand(time(NULL));


	head = qee_create(CONTENT("`"));
	PACK("Create a queue, with the contents: NULL; \"`\";");


	char content[2] = "`";
	short i, j, max;

	for(i = 0; i < 4; i++){
		content[0]++;
		qee_add_item(&head, CONTENT(content), false);
	}
	PACK("Add new items to queue");


	for(i = 0; i < 4; i++){
		RAND_CTT;

		max = 1 + rand() % 5;

		for(j = 0; j < max; j++)
			qee_add_item(&head, CONTENT(content), true);
	}
	PACK("Update items quantity");


	WARN("Get 5-9 random items");
	Queue *buf;
	max = rand() % 5 + 5;

	for(i = 0; i < max; i++){
		RAND_CTT;
		buf = qee_get_item(head, content);
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
	printf("Content #0:     \"%s\"\n", item->content[0]);
	printf("Content #1:     \"%s\"\n", item->content[1]);
	printf("Quantity:        %d\n",    item->quantity);
	printf("Next (Queue):    %p\n",    item->next);

	putchar('\n');
}

static void pqueue(Queue *item){
	if(item == NULL)
		return;

	pqueue(item->next);

	pitem(item);
}
