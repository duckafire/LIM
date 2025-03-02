#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "string-plus.h"
#include "queue.h"

static bool qee_add_item_return;
static bool qee_add_item_quant_upped;
static Queue *new_item;
static Queue *tmp_item;
static bool bigger_to_lower_is_allow = true;
static IS_DUPLIC treat_duplicated_item;

#define CK_QTT_AND_LEN(n0, n1)        ( \
	n0->quantity < n1->quantity ||    ( \
		n0->quantity == n1->quantity && \
		string_length(n0->ident) < string_length(n1->ident) \
		) \
	)

Queue* qee_create(char *ident, char *table_key, char *nick, bool is_const){
	new_item = malloc(sizeof(Queue));

	new_item->ident     = string_copy(ident);
	new_item->table_key = string_copy(table_key);
	new_item->nick      = string_copy(nick);
	new_item->quantity  = 0;
	new_item->is_const  = is_const;
	new_item->next      = NULL;

	return new_item;
}

bool qee_add_item(Queue **head, char *ident, char *table_key, char *nick, bool is_const, IS_DUPLIC treat){
	new_item = qee_create(ident, table_key, nick, is_const);

	treat_duplicated_item = treat;
	qee_add_item_return = false;
	qee_add_item_quant_upped = false;
	*head = insert_item(*head);

	if(qee_add_item_quant_upped && bigger_to_lower_is_allow)
		*head = ordenate_queue(*head);

	return qee_add_item_return;
}

static Queue* insert_item(Queue *item){
	if(item == NULL){
		qee_add_item_return = true;
		return new_item;
	}

	if(bigger_to_lower_is_allow && CK_QTT_AND_LEN(item, new_item)){
		qee_add_item_return = true;
		new_item->next = item;
		return new_item;

	}else if(string_compare(item->ident, new_item->ident)){
		if(treat_duplicated_item == QEE_UP_QUANT){
			qee_add_item_quant_upped = true;
			(item->quantity)++;
		}

		if(treat_duplicated_item == QEE_DROP){
			free_item(new_item);
			return item;
		}

		// NOTE: if the last condition aren't
		// `true`, the item'll be inserted
		// after other clones
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

Queue* qee_get_item(Queue *item, char *ident){
	if(item == NULL)
		return NULL;

	if(string_compare(ident, item->ident))
		return item;
	
	return qee_get_item(item->next, ident);
}

void qee_free_queue(Queue *item){
	if(item == NULL)
		return;

	qee_free_queue(item->next);

	free_item(item);
}

static void free_item(Queue *item){
	free(item->ident);
	free(item->table_key);
	free(item->nick);
	free(item);
}

void qee_bigger_to_lower(bool allow){
	bigger_to_lower_is_allow = allow;
}
