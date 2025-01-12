#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../string-plus.h"
#include "buf-man.h"

static bool update_node_quantity = false;
static BinNode *new_node;
static BinNode *tmp_node;
static bool bin3_add_node_status;

BinNode* bin3_create(char id){
	new_node = malloc(sizeof(BinNode));
	new_node->id = id;
	new_node->height = 0;
	new_node->quantity = 0;

	new_node->content[0] = NULL;
	new_node->content[1] = NULL;

	new_node->left  = NULL;
	new_node->right = NULL;
	new_node->next  = NULL;

	return new_node;
}

bool bin3_add_node(BinNode **root, char id, char *content0, char *content1, bool upQtt){
	new_node = bin3_create(id);
	new_node->content[0] = string_copy(content0);
	new_node->content[1] = string_copy(content1);

	update_node_quantity = upQtt;
	bin3_add_node_status = false;
	*root = insert_node(*root);

	return bin3_add_node_status;
}

static BinNode* insert_node(BinNode *node){
	if(node == NULL){
		bin3_add_node_status = true;
		return new_node;
	}

	if(new_node->id < node->id){
		node->left = insert_node(node->left);

	}else if(new_node->id > node->id){
		node->right = insert_node(node->right);

	}else{
		if(update_node_quantity && string_compare(node->content[1], new_node->content[1])){
			free_node(new_node);
			(node->quantity)++;

			return node;
		}

		node->next = insert_node(node->next);
		return node;
	}

	node->height = update_node_height(node);
	node = avl_balance(node);

	return node;
}

BinNode* bin3_get_node(BinNode *node, char id, char *content1){
	if(id == node->id){
		if(string_compare(content1, node->content[1]))
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

	free_node(node);
}

static void free_node(BinNode *node){
	free(node->content[0]);
	free(node->content[1]);
	free(node);
}


#define GET_SAFE_NODE_HEIGHT(n) ((n == NULL) ? 0 : n->height)

static short max(short a, short b){
	return ((a > b) ? a : b);
}

static short update_node_height(BinNode *node){
	if(node == NULL)
		return 0;

	return max(update_node_height(node->left), update_node_height(node->right)) + 1;
}

#define AVL_SINGLE_ROTATION(root, main_dir, dir_1)                                       \
	BinNode *son, *niece;                                                                \
	son   = root->dir_1;                                                                 \
	niece = son->main_dir;                                                               \
	son->main_dir = root;                                                                \
	root->dir_1   = niece;                                                               \
	root->height = max(update_node_height(root->left), update_node_height(root->right)); \
	son->height  = max(update_node_height(son->left),  update_node_height(son->right));  \
	return son

static short avl_balancement_factor(BinNode *root){
	if(root == NULL)
		return 0;

	return GET_SAFE_NODE_HEIGHT(root->left) - GET_SAFE_NODE_HEIGHT(root->right);
}

static BinNode* avl_balance(BinNode *root){
	// avl balancement status
	short avl_root  = avl_balancement_factor(root);
	short avl_left  = avl_balancement_factor(root->left);
	short avl_right = avl_balancement_factor(root->right);

	if(avl_root < -1 && avl_right <= 0)
		return avl_rotation_left(root);

	if(avl_root >  1 && avl_left >= 0)
		return avl_rotation_right(root);

	if(avl_root >  1 && avl_left < 0)
		return avl_rotation_left_right(root);

	if(avl_root < -1 && avl_right > 0)
		return avl_rotation_right_left(root);

	return root;
}

static BinNode* avl_rotation_left(BinNode *node){
	AVL_SINGLE_ROTATION(node, left, right);
}

static BinNode* avl_rotation_right(BinNode *node){
	AVL_SINGLE_ROTATION(node, right, left);
}

static BinNode* avl_rotation_left_right(BinNode *node){
	node->left = avl_rotation_left(node->left);
	return avl_rotation_right(node);
}

static BinNode* avl_rotation_right_left(BinNode *node){
	node->right = avl_rotation_right(node->right);
	return avl_rotation_left(node);
}
