#include <stdlib.h>
#include <stdio.h>

#include "linkedlist.h"

// Partie donnée aux étudiants
list * listCreate() {
	return NULL;
}

list * listAdd(list * l, int v) {
	list * new = (list *)malloc(sizeof(list));
	new->value = v;
	new->next = l;
	return new;
}

int listSize(list * l) {
	int n = 0;
	while (l!=NULL) {
		n = n+1;
		l = l->next;
	}
	return n;
}

void listDisplay(list * l) {
	printf("(");
	while (l) {
		printf("%d", l->value);
		if (l->next) {
			printf(", ");
		}
		l = l->next;
	}
	printf(")\n");
}

list * listRemove(list * l) {
	list * head;
	if (l==NULL) {
		return NULL;
	}
	else {
		head = l->next;
		free(l);
		return head;
	}
}

void listFree(list * l) {
	while (l) {
		l = listRemove(l);
	}
}

list* listInverse(list* l){
    list* l_copy = listCreate();
    while (l){
        l_copy = listAdd(l_copy,l->value);
        l = l->next;
    }
    return l_copy;
}

list* listCopy(list* l){
    list* temp = listInverse(l);
    list* result = listInverse(temp);
    listFree(temp);
    return result;
}