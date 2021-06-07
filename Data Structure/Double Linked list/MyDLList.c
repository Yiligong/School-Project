/*
 ============================================================================
 Name        : MyDLList.c
 Author      : Yiligong
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define BUFFERSIZE 10000

// all the basic data structures and functions are included in this template
// you can add your own auxiliary functions as you like

// data structures representing DLList

// data type for nodes
typedef struct DLListNode {
	int  value;  // value (int) of this list item
	struct DLListNode *prev;
	// pointer previous node in list
	struct DLListNode *next;
	// pointer to next node in list
} DLListNode;

//data type for doubly linked lists
typedef struct DLList{
	int  size;      // count of items in list
	DLListNode *first; // first node in list
	DLListNode *last;  // last node in list
} DLList;

// create a new DLListNode
DLListNode *newDLListNode(int it)
{
	DLListNode *new;
	new = malloc(sizeof(DLListNode));
	assert(new != NULL);
	new->value = it;
	new->prev = new->next = NULL;
	return new;
}

// create a new empty DLList
DLList *newDLList()
{
	DLList *L;

	L = malloc(sizeof (struct DLList));
	assert (L != NULL);
	L->size = 0;
	L->first = NULL;
	L->last = NULL;
	return L;
}

// create a DLList from a text file
// put your time complexity analysis for CreateDLListFromFileDlist() here
// time complexity is O(n) if having n nodes.
DLList *CreateDLListFromFileDlist(const char *filename)
{
	FILE *fp;
	int num;
	int buffer[BUFFERSIZE];
	DLList *FileDList = newDLList();
	if (filename == "stdin") {
		while (fgets(buffer, BUFFERSIZE, stdin) && sscanf(buffer, "%d", &num) == 1) { //O(n) if having n nodes
			DLListNode *curNode = newDLListNode(num);
			if (FileDList->first == 0) {
				FileDList->first = curNode;
				FileDList->last = curNode;
			}
			else {
				FileDList->last->next = curNode;
				curNode->prev = FileDList->last;
				FileDList->last = curNode;
			}
		}
	}
	else {
		fp = fopen(filename,"r");
		while (fscanf(fp, "%d", &num) != EOF) { //O(n) if having n nodes
			DLListNode *curNode = newDLListNode(num);
			if (FileDList->first == 0) {
				FileDList->first = curNode;
				FileDList->last = curNode;
			}
			else {
				FileDList->last->next = curNode;
				curNode->prev = FileDList->last;
				FileDList->last = curNode;
			}
		}
		fclose(fp);
	}
	return FileDList;
 // put your code here

}

// clone a DLList
// put your time complexity analysis for cloneList() here
// time complexity is O(n) if having n nodes
DLList *cloneList(DLList *u)
{
	DLList *cloneDList = newDLList();
	DLListNode *p;
	for (p = u->first; p != NULL; p = p->next) { //O(n) if having n nodes
		int value = p->value;
		DLListNode *cloneNode = newDLListNode(value);
		if (cloneDList->first == 0) {
			cloneDList->first = cloneNode;
			cloneDList->last = cloneNode;
		}
		else {
			cloneDList->last->next = cloneNode;
			cloneNode->prev = cloneDList->last;
			cloneDList->last = cloneNode;
		}

	}
	return cloneDList;
 // put your code here

}

// compute the union of two DLLists u and v
// time complexity is O(n^2)
// if u has m nodes, v has f nodes. The union list without moving duplicate has m + f nodes, let's say
// it equals to n
DLList *setUnion(DLList *u, DLList *v)
{
	DLListNode *node1, *node2, *node3;
	DLList *unionDL = newDLList();
	DLList *copyLU = cloneList(u); //O(m)
	DLList *copyLV = cloneList(v); //O(f)

	copyLU->last->next = copyLV->first;
	copyLV->first->prev = copyLU->last;
	unionDL->first = copyLU->first;
	unionDL->last = copyLV->last;
	for (node1 = unionDL->first; node1 != NULL; node1 = node1->next) {   //O(n) if having n nodes
		for (node2 = node1->next; node2 != NULL; node2 = node2->next) {  //O(n^2) if having n nodes
			if (node1->value == node2->value) {
				node3 = node2;
				if (node2->next != NULL) {
					node2->prev->next = node2->next;
					node2->next->prev = node2->prev;
				}
				else {
					node2->prev->next = NULL;
				}
				node3 = NULL;
			}
		}
	}
	return unionDL;
	// put your code here
}

// compute the insection of two DLLists u and v
// put your time complexity analysis for intersection() here
// time complexity is O(m * f) if having n nodes
// if u has m nodes, v has f nodes.
DLList *setIntersection(DLList *u, DLList *v)
{
	DLListNode *node1, *node2, *newNode;
	DLList *interDL = newDLList();
	for (node1 = u->first; node1 != NULL; node1 = node1->next) {      //O(m)
		for (node2 = v->first; node2 != NULL; node2 = node2->next) {  //O(m * f)
			if (node1->value == node2->value) {
				newNode = newDLListNode(node1->value);
				if (interDL->first == 0) {
					interDL->first = newNode;
					interDL->last = newNode;
				}
				else {
					interDL->last->next = newNode;
					newNode->prev = interDL->last;
					interDL->last = newNode;
				}
			}
		}
	}
	return interDL;
	// put your code here
}

// free up all space associated with list
// put your time complexity analysis for freeDLList() here
// time complexity is O(n) if having n nodes
void freeDLList(DLList *L)
{
	DLListNode *node1;
	node1 = L->first;
	free(L);
	while (node1 != NULL) {    //O(n) if having n nodes
		DLListNode *node2;
		node2 = node1->next;
		free(node1);
		node1 = node2;

	}

	// put your code here
}


// display items of a DLList
// put your time complexity analysis for printDDList() here
// time complexity is O(n) if having n nodes
void printDLList(DLList *u)
{
	DLListNode *p;
	for (p = u->first; p != NULL; p = p->next) {   //O(n) if having n nodes
		printf("%d\n", p->value);
	}
	// put your code here
}

int main()
{
 DLList *list1, *list2, *list3, *list4;

 list1=CreateDLListFromFileDlist("File1.txt");
 printDLList(list1);

 list2=CreateDLListFromFileDlist("File2.txt");
 printDLList(list2);

 list3=setUnion(list1, list2);
 printDLList(list3);

 list4=setIntersection(list1, list2);
 printDLList(list4);

 freeDLList(list1);
 freeDLList(list2);
 freeDLList(list3);
 freeDLList(list4);

 printf("please type all the integers of list1\n");
 list1=CreateDLListFromFileDlist("stdin");

 printf("please type all the integers of list2\n");
 list2=CreateDLListFromFileDlist("stdin");

 list3=cloneList(list1);
 printDLList(list3);
 list4=cloneList(list2);
 printDLList(list4);

 freeDLList(list1);
 freeDLList(list2);
 freeDLList(list3);
 freeDLList(list4);

 return 0;
}
