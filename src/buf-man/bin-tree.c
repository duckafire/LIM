#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "struct.h"
#include "methods.h"

static bool update_node_quantity = false;
static BinNode *new_node;
static BinNode *tmp_node;

BinNode* bin3_create(char id){
	new_node = malloc(sizeof(BinNode));
	new_node->id = id;
	new_node->quantity = 0;

	new_node->content[0] = NULL;
	new_node->content[1] = NULL;

	new_node->left  = NULL;
	new_node->right = NULL;
	new_node->next  = NULL;

	return new_node;
}

bool bin3_add_node(BinNode *root, char id, char *content0, char *content1, bool upQtt){
	new_node = bin3_create(id);
	new_node->content[0] = content0;
	new_node->content[1] = content1;

	update_node_quantity = upQtt;
	return bin3_insert_node(root);
}

static bool bin3_insert_node(BinNode *node){
	if(new_node->id == node->id){

		if(update_node_quantity && strcmp(node->content[1], new_node->content[1]) == 0){
			bin3_free_node(new_node);
			(node->quantity)++;

			return false;
		}

		if(node->next == NULL){
			node->next = new_node;
			return true;
		}

		return bin3_insert_node(node->next);
	}

	if(new_node->id < node->id){
		if(node->left == NULL){
			node->left = new_node;
			return true;
		}

		return bin3_insert_node(node->left);
	}

	if(node->right == NULL){
		node->right = new_node;
		return true;
	}

	return bin3_insert_node(node->right);
}

BinNode* bin3_get_node(BinNode *node, char id, char *content1){
	if(id == node->id){
		if(strcmp(content1, node->content[1]) == 0)
			return node;

		return bin3_get_node(node->next, id, content1);
	}

	if(id < node->id)
		return bin3_get_node(node->left, id, content1);

	return bin3_get_node(node->right, id, content1);
}

void bin3_free_tree(BinNode *node){
	if(node == NULL)
		return;

	bin3_free_tree(node->left);
	bin3_free_tree(node->right);
	bin3_free_tree(node->next);

	bin3_free_node(node);
}

static void bin3_free_node(BinNode *node){
	free(node->content[0]);
	free(node->content[1]);
	free(node);
}
