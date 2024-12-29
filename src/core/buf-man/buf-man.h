#ifndef BUF_MAN_BUF_MAN
#define BUF_MAN_BUF_MAN

#include <stdio.h>
#include <stdbool.h>

typedef struct BinNode{
	char id;
	char *content[2]; // #1 is single
	unsigned short quantity;
	struct BinNode *left, *right, *next;
}BinNode;

typedef struct Queue{
	char *content[2];
	unsigned short quantity;
	struct Queue *next;
}Queue;
BinNode* bin3_create(char id);
bool bin3_add_node(BinNode *root, char id, char *content0, char *content1, bool upQtt);
static bool bin3_insert_node(BinNode *node);
BinNode* bin3_get_node(BinNode *node, char id, char *content1);
void bin3_free_tree(BinNode *node);
static void bin3_free_node(BinNode *node);

Queue* qee_create(char *content0, char *content1);
bool qee_add_item(Queue **head, char *content0, char *content1, bool upQtt);
static bool qee_insert_item(Queue *mom, Queue *son);
Queue *qee_get_item(Queue *item, char *content1);
void qee_free_queue(Queue *item);
static void qee_free_item(Queue *item);

Queue* convert_bin3_to_qee(BinNode *tree);
static void build_bin3_to_qee(BinNode *node);

#endif
