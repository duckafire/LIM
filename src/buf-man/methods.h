#ifndef BUF_MAN_METHODS
#define BUF_MAN_METHODS

#include <stdbool.h>
#include "struct.h"

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

FromSrcRoot* srcb_create(void);
static FromSrcFunc* srcb_create_func_env(char *funcName);
void srcb_add_func_env(FromSrcRoot *global, char *funcName);
static void srcb_insert_func_env(FromSrcFunc *func);
void srcb_remove_func_env(FromSrcRoot *global, char *funcName);
void srcb_free_func_env(FromSrcFunc *func, FromSrcFunc **tmp);
FILE* srcb_get_buf(FromSrcRoot *global, short bufId, char *funcName);
void srcb_fseek_set_all(FromSrcRoot *global);
static srcb_buf_id srcb_convert_bufId(short *bufId);
void srcb_write_in(FromSrcRoot *global, short bufId, char *content, char *funcName);
void srcb_write_in_order(FromSrcRoot *global, short content);
bool srcb_get_order(FromSrcRoot *global, short *bufId);
void srcb_redo_last_getted(FromSrcRoot *global, short bufId, char *contentGetted, char *funcName);
void srcb_free(FromSrcRoot *global);

#endif
