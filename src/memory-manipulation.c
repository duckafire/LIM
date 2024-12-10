#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "heads.h"

void mm_stringInit(char **buf){
	*buf = malloc(sizeof(char));
	*buf[0] = '\0';
}

void mm_stringAdd(char **buf, char c){
	unsigned short len = strlen(*buf);

	char *tmp;
	tmp = malloc(len + sizeof(char));
	strcpy(tmp, *buf);

	free(*buf);
	*buf = malloc(len + (sizeof(char) * 2));

	strcpy(*buf, tmp);
	(*buf)[len] = c;
	(*buf)[len + 1] = '\0';

	free(tmp);
}

void mm_stringEnd(char **buf, bool restart){
	if(*buf == NULL)
		return;

	free(*buf);
	*buf = NULL;

	if(restart)
		mm_stringInit(buf);
}


BinaryNode* mm_treeInit(char id){
	return mm_treeCreateNode(id, NULL, NULL);
}

void mm_treeNewNode(BinaryNode *root, char id, char *ctt0, char *ctt1, bool upQtt){
	BinaryNode *node;
	node = mm_treeCreateNode(id, ctt0, ctt1);

	mm_treeInsertItem(root, node, upQtt, false);
}

static BinaryNode* mm_treeCreateNode(char id, char *ctt0, char *ctt1){
	char *ctts[2] = {ctt0, ctt1};
	BinaryNode *new;

	new = malloc(sizeof(BinaryNode));
	new->id = id;
	new->content[0] = t_allocAndCopy(ctt0);
	new->content[1] = t_allocAndCopy(ctt1);
	new->quantity = 0; //  "true quantity": current-quantity + 1
	new->left = NULL;
	new->right = NULL;
	new->next = NULL;

	return new;
}

static void mm_treeInsertItem(BinaryNode *node, BinaryNode *new, bool upQtt, bool toQueue){
	// binary queue
	if(new->id == node->id){
		if(upQtt && t_strcmp3(node->content[1], new->content[1])){ // function
			(node->quantity)++;
			mm_treeFreeNodeAndQueueItem(new, NULL);
			return;
		}
		
		if(node->next == NULL)
			node->next = new;
		else
			mm_treeInsertItem(node->next, new, upQtt, true);

		return;
	}

	// binary node
	if(new->id < node->id){
		if(node->left == NULL){
			node->left = new;
			return;
		}

		mm_treeInsertItem(node->left, new, upQtt, false);
		return;
	}

	if(node->right == NULL){
		node->right = new;
		return;
	}

	mm_treeInsertItem(node->right, new, upQtt, false);
}

BinaryNode* mm_treeGetContent(BinaryNode *root, char id){
	if(root == NULL)
		return NULL;

	if(id != root->id){
		BinaryNode *node;
		if((node = mm_treeGetContent(root->left,  id)) != NULL) return node;
		if((node = mm_treeGetContent(root->right, id)) != NULL) return node;
		if((node = mm_treeGetContent(root->next,  id)) != NULL) return node;
	}

	return root;
}

void mm_treeEnd(BinaryNode **root){
	if(*root == NULL)
		return;

	mm_treeEnd(&(*root)->left);
	mm_treeEnd(&(*root)->right);
	mm_treeEnd(&(*root)->next);

	mm_treeFreeNodeAndQueueItem(*root, NULL);
}

void mm_treeFreeNodeAndQueueItem(BinaryNode *node, Queue *item){
	for(short i = 0; i < 2; i++){
		if(node != NULL && node->content[i] != NULL)
			free(node->content[i]);

		if(item != NULL && item->content[i] != NULL)
			free(item->content[i]);
	}

	if(node != NULL)
		free(node);

	if(item != NULL)
		free(item);
}


void mm_queueInsertItem(Queue **head, unsigned short quantity, char *ctt0, char *ctt1, bool upQtt){
	Queue *new;
	new = mm_queueNewItem(quantity, ctt0, ctt1);

	if(*head == NULL){
		*head = new;
		return;
	}

	if(t_strcmp3(new->content[1], (*head)->content[1])){
		if(upQtt)
			((*head)->quantity)++;
		mm_treeFreeNodeAndQueueItem(NULL, new);
		return;
	}
		
	if(new->quantity == (*head)->quantity){
		if(!upQtt){
			mm_queueInsertInBody(*head, (*head)->next, new, upQtt);
			return;
		}

		unsigned int newLen, headLen;
		mm_queueContentsLength(&newLen, &headLen, new, *head);

		if(newLen >= headLen){
			new->next = *head;
			*head = new;
			return;
		}
	}
	
	if(new->quantity > (*head)->quantity){
		new->next = *head;
		*head = new;
		return;
	}

	mm_queueInsertInBody(*head, (*head)->next, new, upQtt);
}

static Queue* mm_queueNewItem(unsigned short quantity, char *ctt0, char *ctt1){
	Queue *new;

	new = malloc(sizeof(Queue));
	new->quantity = quantity;
	new->content[0] = t_allocAndCopy(ctt0);
	new->content[1] = t_allocAndCopy(ctt1);
	new->next = NULL;

	return new;
}

static void mm_queueContentsLength(unsigned int *v0, unsigned int *v1, Queue *i0, Queue *i1){
	*v0 = t_strlen2(i0->content[0]) + t_strlen2(i0->content[1]);
	*v1 = t_strlen2(i1->content[0]) + t_strlen2(i1->content[1]);
}

static void mm_queueInsertInBody(Queue *mom, Queue *son, Queue *new, bool upQtt){
	if(son == NULL){
		mom->next = new;
		return;
	}

	if(t_strcmp3(new->content[1], son->content[1])){
		if(upQtt)
			(son->quantity)++;
		mm_treeFreeNodeAndQueueItem(NULL, new);
		return;
	}

	if(new->quantity == son->quantity){
		if(!upQtt){
			mm_queueInsertInBody(son, son->next, new, upQtt);
			return;
		}
		
		unsigned int newLen, sonLen;
		mm_queueContentsLength(&newLen, &sonLen, new, son);

		if(newLen >= sonLen){
			new->next = son;
			mom->next = new;
			return;
		}

		mm_queueInsertInBody(son, son->next, new, upQtt);
		return;
	}

	if(new->quantity > son->quantity){
		new->next = son;
		mom->next = new;
		return;
	}

	mm_queueInsertInBody(son, son->next, new, upQtt);
}
