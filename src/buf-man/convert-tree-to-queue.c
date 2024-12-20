#include <stdbool.h>
#include <stddef.h>
#include "struct.h"
#include "methods.h"

static Queue *new_queue;

Queue* convert_bin3_to_qee(BinNode *tree){
	new_queue = qee_create(tree->content[0], tree->content[1]);

	build_bin3_to_qee(tree->left);
	build_bin3_to_qee(tree->right);
	build_bin3_to_qee(tree->next);

	return new_queue;
}

static void build_bin3_to_qee(BinNode *node){
	if(node == NULL)
		return;

	build_bin3_to_qee(node->left);
	build_bin3_to_qee(node->right);
	build_bin3_to_qee(node->next);

	qee_add_item(&new_queue, node->content[0], node->content[1], false);
}
