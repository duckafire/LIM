#ifndef BUF_MAN_STRUCT
#define BUF_MAN_STRUCT

#include <stdio.h>
#include "indexes.h"

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

typedef struct FromSrcFunc{
	char *name;
	FILE *bufs[FROMSRC_TOTAL_LOCAL_BUFS];
	struct FromSrcFunc *next;
}FromSrcFunc;

typedef struct FromSrcRoot{
	FILE *bufs[FROMSRC_TOTAL_GLOBAL_BUFS];
	FromSrcFunc *next;
}FromSrcRoot;

#endif
