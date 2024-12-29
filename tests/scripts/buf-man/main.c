#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "../../../src/core/buf-man/buf-man.h"

static void bin_tree_c(void);
static void queue_c(void);
static void convert_tree_to_queue_c(void);

static void printf_node(BinNode *node, char *name);
static void printf_item(Queue *item, char *name);

static BinNode *tree;
static Queue *head;

int main(int argc, char *argv[]){
	if(strcmp(argv[1], "bin-tree.c") == 0)
		bin_tree_c();

	else if(strcmp(argv[1], "queue.c") == 0)
		queue_c();

	else
		convert_tree_to_queue_c();

	return 0;
}

static void printf_node(BinNode *node, char *name){
	if(name != NULL)
		printf("%s\n", name);

	printf("Index (id):   '%c'\n", node->id);
	printf("Content #0: \"%s\"\n", node->content[0]);
	printf("Content #1: \"%s\"\n", node->content[1]);
	printf("Quantity:       %d\n", node->quantity);
	printf("Next (BinNode): %p\n", node->next);
	putchar('\n');
}

static void printf_item(Queue *item, char *name){
	if(name != NULL)
		printf("%s\n", name);

	printf("Content #0:   %s\n", item->content[0]);
	printf("Content #1:   %s\n", item->content[1]);
	printf("Quantity:     %d\n", item->quantity);
	printf("Next (queue): %p\n", item->next);
	putchar('\n');
}

static void bin_tree_c(void){
	tree = bin3_create('`');
	printf_node(tree, "Create tree");
	bin3_free_tree(tree);
}

static void queue_c(void){
	head = qee_create("content-#0", "content-#1");
	printf_item(head, "Create head");
	qee_free_queue(head);
}

static void convert_tree_to_queue_c(void){
	printf("Work in progress!\n\n");
	//bin_tree_c();
	//queue_c();
}
