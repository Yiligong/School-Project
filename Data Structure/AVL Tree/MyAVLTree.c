#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#define BUFFERSIZE 100

// all the basic data structures and functions are included in this template
// you can add your own auxiliary functions as you like

// data type for avl tree nodes
typedef struct AVLTreeNode {
	int key; //key of this item
	int  value;  //value (int) of this item
	int height; //height of the subtree rooted at this node
	struct AVLTreeNode *parent; //pointer to parent
	struct AVLTreeNode *left; //pointer to left child
	struct AVLTreeNode *right; //pointer to right child
} AVLTreeNode;

//data type for AVL trees
typedef struct AVLTree{
	int  size;      // count of items in avl tree
	AVLTreeNode *root; // root
} AVLTree;

// create a new AVLTreeNode
AVLTreeNode *newAVLTreeNode(int k, int v )
{
	AVLTreeNode *new;
	new = malloc(sizeof(AVLTreeNode));
	assert(new != NULL);
	new->key = k;
	new->value = v;
	new->height = 0; // height of this new node is set to 0
	new->left = NULL; // this node has no child
	new->right = NULL;
	new->parent = NULL; // no parent
	return new;
}

// create a new empty avl tree
AVLTree *newAVLTree()
{
	AVLTree *T;
	T = malloc(sizeof (AVLTree));
	assert (T != NULL);
	T->size = 0;
	T->root = NULL;
	return T;
}

int height(AVLTreeNode *n) {
	if (n == NULL)
		return 0;
	return n->height;
}
int max(int a, int b) {
	if (a > b)
		return a;
	return b;
}

AVLTreeNode *leftRotation(AVLTreeNode *z) {
	AVLTreeNode *y, *x;
	y = z->right;
	x = y->left;
	y->left = z;
	z->parent = y;
	z->right = x;
	if (z->left == NULL && z->right == NULL) {
		z->height = 0;
	}
	else
		z->height = max(height(z->left), height(z->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;
	return y;
}

AVLTreeNode *rightRotation(AVLTreeNode *z) {
	AVLTreeNode *y, *x;
	y = z->left;
	x = y->right;
	y->right = z;
	z->parent = y;
	z->left = x;
	if (z->left == NULL && z->right == NULL) {
			z->height = 0;
	}
	else
		z->height = max(height(z->left), height(z->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;
	return y;
}

int isbalance(AVLTreeNode *root) {
	int isbalance;
	if ((root->left == NULL) && (root->right != NULL))
		isbalance = -1 - height(root->right);
	else if ((root->left != NULL) && (root->right == NULL))
		isbalance = height(root->left) - (-1);
	else
		isbalance = height(root->left) - height(root->right);
	return isbalance;
}

AVLTreeNode *balanceAVLTree(AVLTreeNode *root,int key,int value,char token) {
	int isbal;
	isbal = isbalance(root);
	if (token == 'i') {
		if (isbal > 1) {
			if ((key > root->left->key)||((key == root->left->key) && (value > root->left->value))) {
				root->left = leftRotation(root->left);
				return rightRotation(root);
			}
			else if ((key < root->left->key)||((key == root->left->key)&&(value < root->left->value))) {
				return rightRotation(root);
			}
		}
		else if (isbal < -1) {
			if ((key < root->right->key)||((key == root->right->key)&&(value < root->right->value))) {
				root->right = rightRotation(root->right);
				return leftRotation(root);
			}
			else if ((key > root->right->key)||((key == root->right->key) && (value > root->right->value))) {
				return leftRotation(root);
			}
		}
		return root;
	}
	else {
		if (isbal > 1) {
			if (isbalance(root->left) > 0) {
				return rightRotation(root);
			}
			else if (isbalance(root->left) < 0) {
				root->left = leftRotation(root->left);
				return rightRotation(root);
			}
		}
		else if (isbal < -1) {
			if (isbalance(root->right) < 0) {
				return leftRotation(root);
			}
			else if (isbalance(root->right) > 0) {
				root->right = rightRotation(root->right);
				return leftRotation(root);
			}
		}
		return root;
	}
}

int flag = 1;
AVLTreeNode *recurCreateNode(AVLTreeNode *root, int key, int value) {
	char token = 'i';
	if (root == NULL) {
		return newAVLTreeNode(key,value);
	}
	if (key < root->key) {
		root->left = recurCreateNode(root->left,key,value);
		root->left->parent = root;
	}
	else if (key > root->key) {
		root->right = recurCreateNode(root->right,key,value);
		root->right->parent = root;
	}
	else if (key == root->key) {
		if (value < root->value) {
			root->left = recurCreateNode(root->left,key,value);
			root->left->parent = root;
		}
		else if (value > root->value) {
			root->right = recurCreateNode(root->right,key,value);
			root->right->parent = root;
		}

		else {
			flag = 0;
			return root;
		}
	}
	if ((root->left == NULL) && (root->right == NULL)) {
		root->height = 0;
	}
	else
		root->height = max(height(root->left),height(root->right)) + 1;
	return balanceAVLTree(root,key,value,token);
}


// put your time complexity analysis of CreateAVLTree() here
//if having n nodes, time complexity is O(n*logn) time
AVLTree *CreateAVLTree(const char *filename)
{
	const char *split = "(,) ";
	char *p;
	int num[2];
	int i = 0;
	char s[BUFFERSIZE];
	char buffer[BUFFERSIZE];
	int key,value;
	FILE *fp;
	AVLTree *FileTree = newAVLTree();
	if (filename == "stdin") {
		while (fgets(buffer, BUFFERSIZE, stdin) && sscanf(buffer, "%[^\n]s", s) == 1) { //take O(n) time
			i = 0;
			p = strtok(s,split);
			while (p != NULL && i < 2){    // take O(2n) time
				num[i] = (int)(*p) - 48;
				p = strtok(NULL,split);
				i += 1;
			}
			FileTree->root = recurCreateNode(FileTree->root, num[0], num[1]); //take O(n*logn) time
			FileTree->size += 1;
		}
	}
	else {
		fp = fopen(filename, "r");
		while (fscanf(fp, "%*c%d%*c%d%*c%*c%*[\n]", &key,&value) != EOF ) { //take O(n) time
			FileTree->root = recurCreateNode(FileTree->root, key, value); //take O(n*logn) time
			FileTree->size += 1;
		}
	}
	return FileTree;
	// put your code here

}

AVLTreeNode *CloneAVLTreeRecur(AVLTreeNode *node) {
	if (node == NULL) {
		return NULL;
	}
	AVLTreeNode *newNode = newAVLTreeNode(node->key,node->value);
	newNode->height = node->height;
	newNode->parent = node->parent;
	newNode->left = CloneAVLTreeRecur(node->left);
	newNode->right = CloneAVLTreeRecur(node->right);
	return newNode;
}


// put your time complexity analysis for CloneAVLTree() here
// if T's size is n, time complexity is O(n).
AVLTree *CloneAVLTree(AVLTree *T)
{

	AVLTree *newTree = newAVLTree();
	newTree->root = CloneAVLTreeRecur(T->root); // take O(n) time
	newTree->size = T->size;
	return newTree;
	// put your code here

}

AVLTreeNode *AVLTreeUnionRecur(AVLTreeNode *root,AVLTreeNode *insertroot) {
	if (insertroot == NULL) {
		return insertroot;
	}
	root = recurCreateNode(root, insertroot->key, insertroot->value); //take O((m+n)*log(m+n)) time
	AVLTreeUnionRecur(root, insertroot->left);
	AVLTreeUnionRecur(root, insertroot->right);
	return root;

}


// put your time complexity for ALVTreesUNion() here
// if T1'size is m, T2's size is n. Time complexity is O((m+n)*log(m+n))
AVLTree *AVLTreesUnion(AVLTree *T1, AVLTree *T2)
{
	AVLTree *unionTree = CloneAVLTree(T1); //take O(m) time
	unionTree->root = AVLTreeUnionRecur(unionTree->root,T2->root); // take O((m+n)*log(m+n) time
	return unionTree;
	//put your code here
}

int count;
AVLTreeNode *AVLTreesIntersectionRecur(AVLTreeNode *node1, AVLTreeNode *node2, AVLTree *interTree) {
	if (node1 == NULL)
		return node1;
	else if (node2 == NULL)
		return node2;
	AVLTreesIntersectionRecur(node1->left,node2,interTree);
	AVLTreesIntersectionRecur(node1,node2->left,interTree);
	if ((node1->key < node2->key) || ((node1->key == node2->key)&&(node1->value < node2->value))) {
		AVLTreesIntersectionRecur(node1->right,node2,interTree);
	}
	else if ((node1->key > node2->key) || ((node1->key == node2->key)&&(node1->value > node2->value))) {
		AVLTreesIntersectionRecur(node1,node2->right,interTree);
	}
	else if (node1->key == node2->key && node1->value == node2->value){
		count += 1;
		interTree->root = recurCreateNode(interTree->root, node1->key, node1->value);
		if (flag == 0) {
			count -= 1;
			flag = 1;
		}
		AVLTreesIntersectionRecur(node1->right,node2->right,interTree);
	}
	return interTree->root;
}

// put your time complexity for ALVTreesIntersection() here
// if T1's size is n, T2's size is m and the intersection tree's size is k.
// the time complexity is O(n+m+klogk)
AVLTree *AVLTreesIntersection(AVLTree *T1, AVLTree *T2)
{
	 AVLTree *interTree = newAVLTree();
	 interTree->root = AVLTreesIntersectionRecur(T1->root, T2->root, interTree);//take O(n+m+klogk) time
	 interTree->size = count;
	 return interTree;
	//put your code here
}

// put the time complexity analysis for InsertNode() here
// if T's size is n, time complexity is O(logn)
int InsertNode(AVLTree *T, int k, int v)
{
	if (T->root == NULL) {
			T->root = newAVLTreeNode(k,v);
			return 1;
		}
	else {
		AVLTreeNode *node= recurCreateNode(T->root,k,v); //take O(logn) time
		if (flag == 0) {
			flag = 1;
			return 0;
		}
		else {
			T->root = node;
			T->size += 1;
			return 1;
		}
	}
	//put your code here
}

AVLTreeNode *InorderPre(AVLTreeNode *n) {
	AVLTreeNode *currentNode = n->right;
	while (currentNode->left != NULL) { //take O(logn) time
		currentNode = currentNode->left;
	}
	return currentNode;
}

int dflag = 0;
AVLTreeNode *DeleteNodeRecur(AVLTreeNode *n, int k, int v) {
	char token = 'd';
	if (n == NULL)
		return n;
	if (k < n->key) {
		n->left = DeleteNodeRecur(n->left,k,v);
	}
	else if (k > n->key) {
		n->right = DeleteNodeRecur(n->right,k,v);
	}
	else {
		if (v < n->value) {
			n->left = DeleteNodeRecur(n->left,k,v);
		}
		else if (v > n->value) {
			n->right = DeleteNodeRecur(n->right,k,v);
		}
		else {
			dflag = 1;
			if (n->left != NULL && n->right != NULL) {
				AVLTreeNode *inorderPreNode = InorderPre(n); // take O(logn) time
				n->key = inorderPreNode->key;
				n->value = inorderPreNode->value;
				n->right = DeleteNodeRecur(n->right,inorderPreNode->key,inorderPreNode->value); //take O(logn) time
			}
			else {
				if (n->left == NULL) {
					AVLTreeNode *rightnode = n->right;
					free(n);
					return rightnode;
				}
				else {
					AVLTreeNode *leftnode = n->left;
					free(n);
					return leftnode;
				}
			}
		}
	}
	if ((n->left == NULL) && (n->right == NULL)) {
			n->height = 0;
		}
		else
			n->height = max(height(n->left),height(n->right)) + 1;
	return balanceAVLTree(n,k,v,token);
}



// put your time complexity for DeleteNode() here
// if T's size is n, time complexity is O(logn)
int DeleteNode(AVLTree *T, int k, int v)
{
	AVLTreeNode *node = DeleteNodeRecur(T->root,k,v); //take O(2logn) time
	if (dflag == 1){
		T->root = node;
		T->size -= 1;
		dflag = 0;
		return 1;
	}
	else
		return 0;
	// put your code here
}

// put your time complexity analysis for Search() here
// if T'size is n, time complexity is O(logn)
AVLTreeNode *Search(AVLTree *T, int k, int v)
{
	AVLTreeNode *currentNode;
	currentNode = T->root;
	while (currentNode != NULL) { //take O(logn) time
		if (currentNode->key == k && currentNode->value == v) {
			return currentNode;
		}
		else {
			if (k < currentNode->key)
				currentNode = currentNode->left;
			else if (k > currentNode->key)
				currentNode = currentNode->right;
			else {
				if (v < currentNode->value)
					currentNode = currentNode->left;
				else
					currentNode = currentNode->right;
			}
		}
	}
	return NULL;
	// put your code here
}

void FreeAVLTreeRe(AVLTreeNode *n) { //take O(n) time
	if (n != NULL) {
		FreeAVLTreeRe(n->left);
		FreeAVLTreeRe(n->right);
		free(n);
	}
}

// put your time complexity analysis for freeAVLTree() here
// if T'size is n, time complexity is O(n) time
void FreeAVLTree(AVLTree *T)
{
	if (T == NULL)
		return;
	FreeAVLTreeRe(T->root); //take O(n) time
	free(T);
	// put your code here
}

void PrintRecursive(AVLTreeNode *nodeT) {
	if (nodeT != NULL) {
		PrintRecursive(nodeT->left);
		printf("(%d,%d),%d\n", nodeT->key, nodeT->value, nodeT->height);
		PrintRecursive(nodeT->right);
	}
}

// put your time complexity analysis for PrintAVLTree() here
// if T'size is n, time complexity is O(n) time
void PrintAVLTree(AVLTree *T)
{
	if (T->root != NULL) {
		AVLTreeNode *nodeT = T->root;
		PrintRecursive(nodeT); //take O(n) time
	}
	// put your code here
}

int main() //sample main for testing
{ int i,j;
 AVLTree *tree1, *tree2, *tree3, *tree4, *tree5, *tree6, *tree7, *tree8;
 AVLTreeNode *node1;
 tree1=CreateAVLTree("stdin");
 PrintAVLTree(tree1);
 FreeAVLTree(tree1);
 //you need to create the text file file1.txt
 // to store a set of items without duplicate items
 tree2=CreateAVLTree("file1.txt");
 PrintAVLTree(tree2);
 printf("tree2 size %d\n", tree2->size);
 tree3=CloneAVLTree(tree2);
 PrintAVLTree(tree3);
 FreeAVLTree(tree2);
 FreeAVLTree(tree3);
 //Create tree4
 tree4=newAVLTree();
 j=InsertNode(tree4, 10, 10);
 for (i=0; i<15; i++)
  {
   j=InsertNode(tree4, i, i);
   if (j==0) printf("(%d, %d) already exists\n", i, i);
  }
  PrintAVLTree(tree4);
  node1=Search(tree4,20,20);
  if (node1!=NULL)
    printf("key= %d value= %d\n",node1->key,node1->value);
  else
    printf("Key 20 does not exist\n");

  for (i=17; i>0; i--)
  {
    j=DeleteNode(tree4, i, i);
	if (j==0)
	  printf("Key %d does not exist\n",i);
    PrintAVLTree(tree4);
  }
 FreeAVLTree(tree4);
 //Create tree5
 tree5=newAVLTree();
 j=InsertNode(tree5, 6, 25);
 j=InsertNode(tree5, 6, 10);
 j=InsertNode(tree5, 6, 12);
 j=InsertNode(tree5, 6, 20);
 j=InsertNode(tree5, 9, 25);
 j=InsertNode(tree5, 10, 25);
 PrintAVLTree(tree5);
 //Create tree6
 tree6=newAVLTree();
 j=InsertNode(tree6, 6, 25);
 j=InsertNode(tree6, 5, 10);
 j=InsertNode(tree6, 6, 12);
 j=InsertNode(tree6, 6, 20);
 j=InsertNode(tree6, 8, 35);
 j=InsertNode(tree6, 10, 25);
 PrintAVLTree(tree6);
 tree7=AVLTreesIntersection(tree5, tree6);
 tree8=AVLTreesUnion(tree5,tree6);
 PrintAVLTree(tree7);
 PrintAVLTree(tree8);
 return 0;
}
