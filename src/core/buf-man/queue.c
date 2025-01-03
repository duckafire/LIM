#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../tools/string-plus.h"
#include "buf-man.h"

static bool qee_add_item_status;
static bool update_item_quantity;
static Queue *new_item;
static Queue *tmp_item;

#define CK_QTT_AND_LEN(n0, n1) \
	(n0->quantity < n1->quantity || (n0->quantity == n1->quantity && string_length(n0->content[1]) < string_length(n1->content[1])))

Queue* qee_create(char *content0, char *content1){
	new_item = malloc(sizeof(Queue));

	new_item->quantity = 0;
	new_item->content[0] = string_copy(content0);
	new_item->content[1] = string_copy(content1);
	new_item->next = NULL;

	return new_item;
}

bool qee_add_item(Queue **head, char *content0, char *content1, bool upQtt){
	new_item = qee_create(content0, content1);

	update_item_quantity = upQtt;
	qee_add_item_status = false;
	*head = insert_item(*head);
	*head = ordenate_queue(*head);

	return qee_add_item_status;
}

static Queue* insert_item(Queue *item){
	if(item == NULL){
		qee_add_item_status = true;
		return new_item;
	}

	if(CK_QTT_AND_LEN(item, new_item)){
		qee_add_item_status = true;
		new_item->next = item;
		return new_item;
	}else if(string_compare(item->content[1], new_item->content[1])){
		if(update_item_quantity)
			(item->quantity)++;

		free_item(new_item);
		return item;
	}

	item->next = insert_item(item->next);
	return item;
}

static Queue* ordenate_queue(Queue *item){
	if(item == NULL || item->next == NULL)
		return item;

	if(CK_QTT_AND_LEN(item, item->next)){
		tmp_item = item->next;
		item->next = tmp_item->next;
		tmp_item->next = item;

		tmp_item->next = ordenate_queue(item);
		return tmp_item;
	}

	item->next = ordenate_queue(item->next);
	return item;
}

Queue* qee_get_item(Queue *item, char *content1){
	if(string_compare(content1, item->content[1]))
		return item;
	
	qee_get_item(item->next, content1);
}

void qee_free_queue(Queue *item){
	if(item == NULL)
		return;

	qee_free_queue(item->next);

	free_item(item);
}

static void free_item(Queue *item){
	free(item->content[0]);
	free(item->content[1]);
	free(item);
}
