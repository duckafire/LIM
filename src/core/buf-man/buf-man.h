#ifndef CORE_BUF_MAN_BUF_MAN
#define CORE_BUF_MAN_BUF_MAN

#include <stdio.h>
#include <stdbool.h>

typedef struct BinNode{
	char id;
	char *content[2]; // #1 is single
	short height;
	unsigned short quantity;
	struct BinNode *left, *right, *next;
}BinNode;

typedef struct Queue{
	char *content[2];
	unsigned short quantity;
	struct Queue *next;
}Queue;

BinNode* bin3_create(char id);
bool bin3_add_node(BinNode **root, char id, char *content0, char *content1, bool upQtt);
static BinNode* insert_node(BinNode *node);
BinNode* bin3_get_node(BinNode *node, char id, char *content1);
void bin3_free_tree(BinNode *node);
static void free_node(BinNode *node);
static short max(short a, short b);
static short update_node_height(BinNode *node);
static short avl_balancement_factor(BinNode *root);
static BinNode* avl_balance(BinNode *root);
static BinNode* avl_rotation_left(BinNode *node);
static BinNode* avl_rotation_right(BinNode *node);
static BinNode* avl_rotation_left_right(BinNode *node);
static BinNode* avl_rotation_right_left(BinNode *node);

Queue* qee_create(char *content0, char *content1);
bool qee_add_item(Queue **head, char *content0, char *content1, bool upQtt);
static Queue* insert_item(Queue *item);
static Queue* ordenate_queue(Queue *item);
Queue *qee_get_item(Queue *item, char *content1);
void qee_free_queue(Queue *item);
static void free_item(Queue *item);

Queue* convert_bin3_to_qee(BinNode *tree, bool upQtt);
static void build_bin3_to_qee(BinNode *node);

#endif
