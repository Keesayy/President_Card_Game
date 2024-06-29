#ifndef LIST_H
#define LIST_H

typedef struct list {
	int value;
	struct list * next;
} list;

list * listCreate();
list * listAdd(list * l, int v);
int listSize(list * l);
void listDisplay(list * l);
list * listRemove(list * l);
void listFree(list * l);
list* listInverse(list* l);
list* listCopy(list* l);

#endif
