#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include "../debug-common.h"
#include "../../../src/core/buf-man/buf-man.h"


static void bin_tree_c(void);
static void queue_c(void);
static void convert_tree_to_queue_c(void);

static void printf_node(BinNode *node);
static void printf_full_tree(BinNode *node);
static void printf_item(Queue *item, char *name);
static void printf_full_queue(Queue *item);

static BinNode *tree;
static Queue *head;

static bool print_messages = true;


#define CONTENT      "content-#0","content-#1"
#define CONTENT_2    "CONTENT-#0","CONTENT-#1"
#define CONTENT_3(b) NULL,b

#define PARAGRAPH    if(print_messages) puts("\n\n")
#define _WARN(str)   if(print_messages) WARN(str)
#define PRINTF(s, i) if(print_messages) printf(s, i)


int main(int argc, char *argv[]){
	CHECK_PROGRAM_ARG_QUANTITY
	srand(time(NULL));

	if(strcmp(argv[1], "bin-tree.c") == 0)
		bin_tree_c();

	else if(strcmp(argv[1], "queue.c") == 0)
		queue_c();

	else if(strcmp(argv[1], "convert-tree-to-queue.c") == 0)
		convert_tree_to_queue_c();

	else{
		INVALID_ARG_TO_MAIN("bin-tree.c\nqueue.c\nconvert-tree-to-queue.c");
	}

	return 0;
}


static void printf_node(BinNode *node){
	if(!print_messages)
		return;

	printf("\"This\" (BinNode): %p\n",  node);
	printf("Index (id):      '%c'\n",   node->id);
	printf("Content #0:      \"%s\"\n", node->content[0]);
	printf("Content #1:      \"%s\"\n", node->content[1]);
	printf("Height:           %d\n",    node->height);
	printf("Quantity:         %d\n",    node->quantity);
	printf("Left (BinNode):   %p\n",    node->left);
	printf("Right (BinNode):  %p\n",    node->right);
	printf("Next (BinNode):   %p\n",    node->next);

	putchar('\n');
}

static void printf_full_tree(BinNode *node){
	if(node == NULL || !print_messages)
		return;

	printf_full_tree(node->left);
	printf_node(node);
	printf_full_tree(node->right);
}

static void bin_tree_c(void){
	_WARN("Create binary tree, with index '`'");
	tree = bin3_create('`');
	printf_full_tree(tree);
	PARAGRAPH;


	_WARN("Add two new nodes, with the respective indexes 'A' and 'a'");
	bin3_add_node(&tree, 'A', CONTENT, false);
	bin3_add_node(&tree, 'a', CONTENT, false);

	// ` A a
	printf_full_tree(tree);
	PARAGRAPH;


	short upQtt, max, lower, i, j;

#define ADD_NODE(c, ctt)                                               \
	if(upQtt == 1)                                                     \
		max = rand() % 5 + 1;                                          \
	for(i = 0; (i == 0 && upQtt == 0) || (upQtt == 1 && i < max); i++) \
		bin3_add_node(&tree, c + lower, ctt, (upQtt == 1))
//#enddef

	for(upQtt = 0; upQtt < 2; upQtt++){

		if(upQtt == 0)
			_WARN("Add four new nodes: x3 to left/right, x1 to next");
		else
			_WARN("Update the quantity of nodes (random)");

		for(lower = 0; lower <= 32; lower += 32){
			ADD_NODE('C', CONTENT);
			ADD_NODE('B', CONTENT);
			ADD_NODE('D', CONTENT);

			for(j = 0; j < 3; j++)
				ADD_NODE('A' + rand() % 4, CONTENT_2);

			// ` A/a B/b C/c D/d
			printf_full_tree(tree);
			PARAGRAPH;
		}
	}

#undef ADD_NODE


	_WARN("Get 5-9 random nodes (A-D/a-d)");

	char id;
	BinNode *buf;
	max = rand() % 5 + 5;

	for(i = 0; i < max; i++){
		id = ((rand() % 2 == 0) ? 'A' : 'a') + rand() % 4;

		buf = bin3_get_node(tree, id, "content-#1");
		
		PRINTF("# node '%c'\n", id);
		printf_node(buf);
	}
	PARAGRAPH;


	if(print_messages)
		bin3_free_tree(tree);
}


static void printf_item(Queue *item, char *name){
	if(!print_messages)
		return;

	if(name != NULL)
		printf("# %s\n", name);

	printf("\"This\" (Queue):  %p\n",  item);
	printf("Content #0:     \"%s\"\n", item->content[0]);
	printf("Content #1:     \"%s\"\n", item->content[1]);
	printf("Quantity:        %d\n",    item->quantity);
	printf("Next (Queue):    %p\n",    item->next);

	putchar('\n');
}

static void queue_c(void){
	_WARN("Create a queue, with the contents: NULL; \"`\";");
	head = qee_create(CONTENT_3("`"));
	printf_item(head, "head");
	PARAGRAPH;


	char content[2] = "`";
	short i, j, max;

#define PRINT_QUEUE(t)                                                         \
	_WARN(t);                                                                   \
	printf_item(head,                         "init");                         \
	printf_item(head->next,                   "init->next");                   \
	printf_item(head->next->next,             "init->next->next");             \
	printf_item(head->next->next->next,       "init->next->next->next");       \
	printf_item(head->next->next->next->next, "init->next->next->next->next"); \
	PARAGRAPH

	for(i = 0; i < 4; i++){
		content[0]++;
		qee_add_item(&head, CONTENT_3(content), false);
	}

	PRINT_QUEUE("Add new items to queue");


#define RAND_CTT content[0] = 'a' + rand() % 4

	for(i = 0; i < 4; i++){
		RAND_CTT;

		max = 1 + rand() % 5;

		for(j = 0; j < max; j++)
			qee_add_item(&head, CONTENT_3(content), true);
	}

	PRINT_QUEUE("Update items quantity");


	_WARN("Get 5-9 random items");

	Queue *buf;
	max = rand() % 5 + 5;

	for(i = 0; i < max; i++){
		RAND_CTT;
		buf = qee_get_item(head, content);

		PRINTF("# item '%c'\n", content[0]);
		printf_item(buf, NULL);
	}
	PARAGRAPH;

#undef PRINTF


	qee_free_queue(head);
}


static void printf_full_queue(Queue *item){
	if(item == NULL)
		return;

	printf_item(item, NULL);

	printf_full_queue(item->next);
}

static void convert_tree_to_queue_c(void){
	print_messages = false;
	bin_tree_c();
	print_messages = true;


	_WARN("Binary tree");
	printf_full_tree(tree);
	PARAGRAPH;


	Queue *no_update, *with_update;
	no_update   = convert_bin3_to_qee(tree, false);
	with_update = convert_bin3_to_qee(tree, true);

	_WARN("Queue NO quantity update)");
	printf_full_queue(no_update);
	PARAGRAPH;

	_WARN("Queue (WITH quantity update)");
	printf_full_queue(with_update);
	PARAGRAPH;


	bin3_free_tree(tree);
	qee_free_queue(no_update);
	qee_free_queue(with_update);
}
