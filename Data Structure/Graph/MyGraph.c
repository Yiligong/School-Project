#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define size 20
// A vertex is a 2D point
// Binomial heap key is the distance from
typedef struct Vertex {
	int x; // x-coordinate
	int y; // y-coordinate
} Vertex;
// each edge is a pair of vertices (end-points)
typedef struct Edge {
	Vertex *p1; // first end point
	Vertex *p2; // second end point
} Edge;
typedef struct VertexNode {
	Vertex *v;
	struct VertexNode *nextVertex;
	struct VertexNode *myArrayNode;
	struct VertexNode *lastNode;
	int explore;
} VertexNode;
typedef struct GraphRep *Graph;
typedef struct GraphRep { // graph header
	VertexNode **vertices; // an array of vertices or a linked list of vertices
	int nV; // #vertices
	int nE; // #edges
} GraphRep;
// A vertex node stores a vertex and other information, and you need to expand this type

Vertex *CreateVertex(x,y) {
	Vertex *v;
	v = malloc(sizeof(Vertex));
	assert(v!=NULL);
	v->x = x;
	v->y = y;
	return v;
}

//The above types serve as a starting point only. You need to expand them and add more types.
// Watch the lecture video between 7:50pm-8:20 or so for my discussion about this assignment
VertexNode *CreateVertexNode(x,y){
	VertexNode *node1;
	Vertex *v1;
	node1 = malloc(sizeof(VertexNode));
	assert(node1!=NULL);
	v1 = CreateVertex(x,y);
	node1->v = v1;
	node1->nextVertex = NULL;
	node1->explore = 0;
	node1->myArrayNode = NULL;
	node1->lastNode = NULL;
	return node1;
}

// Add the time complexity analysis of CreateEmptyGraph() here
// if we allocate the size of an array previously, for example if the size is N.
// the time complexity is O(N)
Graph CreateEmptyGraph()
{
	GraphRep *newGraph;
	newGraph = malloc(sizeof(GraphRep));
	assert(newGraph!=NULL);
	newGraph->nE = 0;
	newGraph->nV = 0;
	newGraph->vertices = malloc(size * sizeof(VertexNode));
	assert(newGraph->vertices!=NULL);
	int i;
	for (i = 0; i < size; i++) {   // time complexity is O(N)
		newGraph->vertices[i] = NULL;
	}
	return newGraph;
}

// Add the time complexity analysis of InsertEdge() here
// if the size of the array is N, the time complexity is O(N+e)
int InsertEdge(Graph g, Edge *e)
{
	int flag = 1;
	int flagv1 = 0;
	int flagv2 = 0;
	if (g->nV == 0) {
		VertexNode *v1 = CreateVertexNode(e->p1->x,e->p1->y);
		VertexNode *v2 = CreateVertexNode(e->p2->x,e->p2->y);
		g->vertices[0] = v1;
		v1->nextVertex = v2;
		v1 = CreateVertexNode(e->p2->x,e->p2->y);
		v2 = CreateVertexNode(e->p1->x,e->p1->y);
		g->vertices[1] = v1;
		v1->nextVertex = v2;
		g->nV += 2;
		g->nE += 1;
		g->vertices[0]->nextVertex->myArrayNode = g->vertices[1];
		g->vertices[1]->nextVertex->myArrayNode = g->vertices[0];
		g->vertices[0]->lastNode = g->vertices[0]->nextVertex;
		g->vertices[1]->lastNode = g->vertices[1]->nextVertex;
	}
	else {
		int i;
		int index1;
		int index2;
		int indexv1;
		int indexv2;
		int x1 = e->p1->x;
		int y1 = e->p1->y;
		int x2 = e->p2->x;
		int y2 = e->p2->y;
		for (i=0;i<size;i++) {   //takes O(N+e) time, if vertices v1 and vertices v2 has already in the array, the worst case is we traverse the entire array
			                     //and traverse all edges of v1 and v2 which is e1 + e1 < e, so the time complexity is O(N+e)
			if (g->vertices[i] == NULL) {
				continue;
			}
			else {
				int cuX = g->vertices[i]->v->x;
				int cuY = g->vertices[i]->v->y;
				VertexNode *nextNode = g->vertices[i]->nextVertex;
				if (cuX == x1 && cuY == y1) {
					indexv1 = i;
					while (nextNode != NULL) {  //takes O(e) time
						g->vertices[i]->lastNode = nextNode;
						if (nextNode->v->x == x2 && nextNode->v->y == y2) {
							flag = 0;
							return 0;
						}
						else {
							if (nextNode->nextVertex == NULL) {
								VertexNode *v2 = CreateVertexNode(x2,y2);
								nextNode->nextVertex = v2;
								flagv1 = 1;
								g->vertices[i]->lastNode = nextNode->nextVertex;
								break;
							}
						}
						nextNode = nextNode->nextVertex;
					}

				}
				else if (cuX == x2 && cuY == y2) {
					indexv2 = i;
					while (nextNode != NULL) {  //takes O(e) time
						g->vertices[i]->lastNode = nextNode;
						if (nextNode->v->x == x1 && nextNode->v->y == y1) {
							flag = 0;
							return 0;
						}
						else {
							if (nextNode->nextVertex == NULL) {
								VertexNode *v1 = CreateVertexNode(x1,y1);
								nextNode->nextVertex = v1;
								flagv2 = 1;
								g->vertices[i]->lastNode = nextNode->nextVertex;
								break;
							}
						}
						nextNode = nextNode->nextVertex;
					}
				}
				else {
					if (g->vertices[i+1] == NULL) {
						index1 = i+1;
						int j;
						for (j=i+2;j<size;j++) {   //takes O(N) time
							if (g->vertices[j] == NULL) {
								index2 = j;
								break;
							}
						}
					}
				}
			}
		}
		if (flagv1 == 0 && flagv2 == 0) {
			VertexNode *v1 = CreateVertexNode(x1,y1);
			VertexNode *v2 = CreateVertexNode(x2,y2);
			g->vertices[index1] = v1;
			v1->nextVertex = v2;
			v2 = CreateVertexNode(x2,y2);
			v1 = CreateVertexNode(x1,y1);
			g->vertices[index2] = v2;
			v2->nextVertex = v1;
			g->nV += 2;
			g->nE += 1;
			g->vertices[index1]->nextVertex->myArrayNode = g->vertices[index2];
			g->vertices[index2]->nextVertex->myArrayNode = g->vertices[index1];
			g->vertices[index1]->lastNode = g->vertices[index1]->nextVertex;
			g->vertices[index2]->lastNode = g->vertices[index2]->nextVertex;
		}
		else if (flagv1 == 1 && flagv2 == 1) {
			g->nE += 1;
			g->vertices[indexv1]->lastNode->myArrayNode = g->vertices[indexv2];
			g->vertices[indexv2]->lastNode->myArrayNode = g->vertices[indexv1];
		}
		else if (flagv1 == 1) {
			VertexNode *v1 = CreateVertexNode(x1,y1);
			VertexNode *v2 = CreateVertexNode(x2,y2);
			g->vertices[index1] = v2;
			v2->nextVertex = v1;
			g->nV += 1;
			g->nE += 1;
			g->vertices[indexv1]->lastNode->myArrayNode = g->vertices[index1];
			g->vertices[index1]->nextVertex->myArrayNode = g->vertices[indexv1];
		}
		else if (flagv2 == 1){
			VertexNode *v1 = CreateVertexNode(x1,y1);
			VertexNode *v2 = CreateVertexNode(x2,y2);
			g->vertices[index1] = v1;
			v1->nextVertex = v2;
			g->nV += 1;
			g->nE += 1;
			g->vertices[indexv2]->lastNode->myArrayNode = g->vertices[index1];
			g->vertices[index1]->nextVertex->myArrayNode = g->vertices[indexv2];
		}
	 }
	return flag;
}


// Add the time complexity analysis of DeleteEdge() here
// if the size of array is N and having e edges, the time complexity is O(N+e)
void DeleteEdge(Graph g, Edge *e)
{
	int x1 = e->p1->x;
	int y1 = e->p1->y;
	int x2 = e->p2->x;
	int y2 = e->p2->y;
	int i;
	for (i=0;i<size;i++){ //takes O(N+e) time, if vertices v1 and vertices v2 in the array, the worst case is we traverse entire array
        				  //and traverse all edges of v1 and v2 which is e1 + e1 < e, so the time complexity is O(N+e)
		if (g->vertices[i] == NULL) {
			continue;
		}
		else {
			int cuX = g->vertices[i]->v->x;
			int cuY = g->vertices[i]->v->y;
			VertexNode *next = g->vertices[i];
			if (cuX == x1 && cuY == y1) {
				while (next->nextVertex != NULL) { //takes O(e) time
					if (next->nextVertex->v->x == x2 && next->nextVertex->v->y == y2) {
						if (next->nextVertex == g->vertices[i]->lastNode) {
							VertexNode *n;
							n = next->nextVertex->nextVertex;
							free(next->nextVertex);
							next->nextVertex = n;
							if (n == NULL) {
								g->vertices[i]->lastNode = next;
							}
							else {
								g->vertices[i]->lastNode = next->nextVertex;
							}
							g->nE -= 1;
							break;
						}
						else {
							VertexNode *n;
							n = next->nextVertex->nextVertex;
							free(next->nextVertex);
							next->nextVertex = n;
							g->nE -= 1;
							break;
						}
					}
					next = next->nextVertex;
				}
			}
			else if (cuX == x2 && cuY == y2) {
				while (next->nextVertex != NULL) {  //takes O(e) time
					if (next->nextVertex->v->x == x1 && next->nextVertex->v->y == y1) {
						if (next->nextVertex == g->vertices[i]->lastNode) {
							VertexNode *n;
							n = next->nextVertex->nextVertex;
							free(next->nextVertex);
							next->nextVertex = n;
							if (n == NULL) {
								g->vertices[i]->lastNode = next;
							}
							else {
								g->vertices[i]->lastNode = next->nextVertex;
							}
							g->nE -= 1;
							break;
						}
						else {
							VertexNode *n;
							n = next->nextVertex->nextVertex;
							free(next->nextVertex);
							next->nextVertex = n;
							break;
						}
					}
					next = next->nextVertex;
				}
			}
			if (g->vertices[i]->nextVertex == NULL) {
				g->vertices[i] = NULL;
				g->nV -= 1;
				free(g->vertices[i]);
			}
		}
	}
}

void RecurDFS(VertexNode *arrayNode) {
	if (arrayNode != NULL) {
		arrayNode->explore = 1;
		VertexNode *currentNode = arrayNode->nextVertex;
		while (currentNode != NULL) {
			if (currentNode->myArrayNode->explore == 0) {
				printf(",(%d,%d)",currentNode->v->x,currentNode->v->y);
				RecurDFS(currentNode->myArrayNode);
				currentNode = currentNode->nextVertex;
			}
			else if (currentNode->myArrayNode->explore == 1) {
				currentNode = currentNode->nextVertex;
			}
		}
	}
}

// Add the time complexity analysis of ReachableVertices() here
// if size of the array is N, the time complexity is O(N+e)
void ReachableVertices(Graph g, Vertex *v)
{
	if (g->nV != 0) {
		int i;
		int vx = v->x;
		int vy = v->y;
		for (i=0;i<size;i++) { // takes O(N+e) time
			if (g->vertices[i] != NULL) {
				int gx = g->vertices[i]->v->x;
				int gy = g->vertices[i]->v->y;
				if (gx == vx && gy == vy) {
					g->vertices[i]->explore = 1;
					printf("(%d,%d)",g->vertices[i]->nextVertex->v->x,g->vertices[i]->nextVertex->v->y);
					RecurDFS(g->vertices[i]->nextVertex->myArrayNode); //takes O(e) time
					break;
				}
			}
		}
	}
	int i;
	for (i=0;i<size;i++) { //takes O(N) time
		if (g->vertices[i] != NULL) {
			g->vertices[i]->explore = 0;
		}
	}
	printf("\n");
}

// Add the time complexity analysis of ShortestPath() here
void ShortestPath(Graph g, Vertex *u, Vertex *v)
{

}

// Add the time complexity analysis of FreeGraph() here
// the time complexity is O(N+e) if the size of array is N and having e edges.
void FreeGraph(Graph g)
{
	int i;
	for (i=0;i<size;i++) { //takes O(N+e) time
		VertexNode *tmp;
		if (g->vertices[i] != NULL) {
			while (g->vertices[i]->nextVertex != NULL) { //takes O(e) time
				tmp = g->vertices[i]->nextVertex;
				g->vertices[i]->nextVertex = g->vertices[i]->nextVertex->nextVertex;
				free(tmp);
			}
			free(g->vertices[i]);
		}
	}
	free(g->vertices);
	free(g);
}

// Add the time complexity analysis of ShowGraph() here
// if size of array is N and having e edges, time complexity is O(N+e)
void ShowGraph(Graph g)
{
	if (g->nV != 0){
		int i;
		for (i=0;i<size;i++) { //takes O(N) time
			if (g->vertices[i] != NULL) {
				g->vertices[i]->explore = 1;
				int vx = g->vertices[i]->v->x;
				int vy = g->vertices[i]->v->y;
				VertexNode *current = g->vertices[i]->nextVertex;
				while (current != NULL) { // takes O(e) time
					if (current->myArrayNode->explore == 0) {
						int ax = current->v->x;
						int ay = current->v->y;
						printf("(%d,%d),(%d,%d) ", vx,vy,ax,ay);
						current = current->nextVertex;
					}
					else
						current = current->nextVertex;
				}
			}
		}
	}
	int i;
	for (i=0;i<size;i++) { //takes O(N) time
		if (g->vertices[i] != NULL) {
			g->vertices[i]->explore = 0;
		}
	}
	printf("\n");
}


int main() //sample main for testing
{
 Graph g1;
 Edge *e_ptr;
 Vertex *v1, *v2;

 // Create an empty graph g1;
 g1=CreateEmptyGraph();

 // Create first connected component
 // Insert edge (0,0)-(0,10)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=0;
 v2->y=10;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (0,0)-(5,6)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=5;
 v2->y=6;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (0, 10)-(10, 10)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=10;
 v2->x=10;
 v2->y=10;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (0,10)-(5,6)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=10;
 v2->x=5;
 v2->y=6;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (0,0)-(5,4)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=5;
 v2->y=4;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (5, 4)-(10, 4)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=5;
 v1->y=4;
 v2->x=10;
 v2->y=4;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (5,6)-(10,6)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=5;
 v1->y=6;
 v2->x=10;
 v2->y=6;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (10,10)-(10,6)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=10;
 v1->y=10;
 v2->x=10;
 v2->y=6;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (10, 6)-(10, 4)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=10;
 v1->y=6;
 v2->x=10;
 v2->y=4;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Create second connected component
 // Insert edge (20,4)-(20,10)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=20;
 v1->y=4;
 v2->x=20;
 v2->y=10;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (20,10)-(30,10)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=20;
 v1->y=10;
 v2->x=30;
 v2->y=10;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 // Insert edge (25,5)-(30,10)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=25;
 v1->y=5;
 v2->x=30;
 v2->y=10;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 if (InsertEdge(g1, e_ptr)==0) printf("edge exists\n");

 //Display graph g1
 ShowGraph(g1);

 // Find the shortest path between (0,0) and (10,6)
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=10;
 v2->y=6;
 ShortestPath(g1, v1, v2);
 free(v1);
 free(v2);

 // Delete edge (0,0)-(5, 6)
 e_ptr = (Edge*) malloc(sizeof(Edge));
 assert(e_ptr != NULL);
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=5;
 v2->y=6;
 e_ptr->p1=v1;
 e_ptr->p2=v2;
 DeleteEdge(g1, e_ptr);
 free(e_ptr);
 free(v1);
 free(v2);

 // Display graph g1
 ShowGraph(g1);

 // Find the shortest path between (0,0) and (10,6)
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=10;
 v2->y=6;
 ShortestPath(g1, v1, v2);
 free(v1);
 free(v2);

 // Find the shortest path between (0,0) and (25,5)
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v2=(Vertex *) malloc(sizeof(Vertex));
 assert(v2 != NULL);
 v1->x=0;
 v1->y=0;
 v2->x=25;
 v2->y=5;
 ShortestPath(g1, v1, v2);
 free(v1);
 free(v2);

 // Find reachable vertices of (0,0)
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v1->x=0;
 v1->y=0;
 ReachableVertices(g1, v1);
 free(v1);

 // Find reachable vertices of (20,4)
 v1=(Vertex*) malloc(sizeof(Vertex));
 assert(v1 != NULL);
 v1->x=20;
 v1->y=4;
 ReachableVertices(g1, v1);
 free(v1);

 // Free graph g1
 FreeGraph(g1);

 return 0;
}

