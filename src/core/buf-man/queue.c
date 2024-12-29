#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "buf-man.h"

static bool update_item_quantity = false;
static Queue *new_item;
static unsigned short new_item_len, son_len;

#define QEE_ADD_AND_INSERT_ITEM_CORE(h)                     \
	if(h == NULL){                                          \
		h = new_item;                                       \
		return true;                                        \
	}                                                       \
	if(strcmp(new_item->content[1], (h)->content[1]) == 0){ \
		if(update_item_quantity)                            \
			((h)->quantity)++;                              \
		qee_free_item(new_item);                            \
		return false;                                       \
	}                                                       \
	new_item_len = strlen(new_item->content[1]);            \
	son_len = strlen((h)->content[1]);                      \
	if(new_item_len > son_len){                             \
		new_item->next = (h);                               \
		(h) = new_item;                                     \
		return true;                                        \
	}                                                       \
	if(new_item->quantity > (h)->quantity){                 \
		new_item->next = (h);                               \
		(h) = new_item;                                     \
		return true;                                        \
	}

Queue* qee_create(char *content0, char *content1){
	new_item = malloc(sizeof(Queue));

	new_item->quantity = 0;
	new_item->content[0] = content0;
	new_item->content[1] = content1;
	new_item->next = NULL;

	return new_item;
}

bool qee_add_item(Queue **head, char *content0, char *content1, bool upQtt){
	new_item = qee_create(content0, content1);

	update_item_quantity = upQtt;

	QEE_ADD_AND_INSERT_ITEM_CORE(*head)

	return qee_insert_item(*head, (*head)->next);
}

static bool qee_insert_item(Queue *mom, Queue *son){
	QEE_ADD_AND_INSERT_ITEM_CORE(mom->next)

	return qee_insert_item(son, son->next);
}

Queue* qee_get_item(Queue *item, char *content1){
	if(strcmp(content1, item->content[1]) == 0)
		return item;
	
	qee_get_item(item->next, content1);
}

void qee_free_queue(Queue *item){
	if(item == NULL)
		return;

	qee_free_queue(item->next);

	qee_free_item(item);
}

static void qee_free_item(Queue *item){
	free(item->content[0]);
	free(item->content[1]);
	free(item);
}

#undef QEE_ADD_AND_INSERT_ITEM_CORE
