#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

struct Edge{
	int y;//end point
	int c;//capacity
	int f;//flow
	struct Edge *next;
};//C type for edge

typedef struct Edge edge;

struct Vertex{
	int x;//id
	int n;//need
	edge *p;
};//C type for vertex

typedef struct Vertex vertex;

struct Graph{
	int v;//#vertices
	int e;//#edges
	vertex *h;//Array 
};//C type for graph

typedef struct Graph graph;

/*All helper functions*/
graph *ReadGraph(char *fname);
void PrintGraph(graph *g);
void PrintGraph2(graph *g);
void addLinkResGraph(graph *g, int x, int y, int c);
int augmentFlow(graph *g, graph *resG, int s, int t);
graph * computeResidualGraph(graph *g);
void ComputeMaxFlow(graph *g, int s, int t);
void NeedBasedFlow(graph *g);
graph *getGraph(int n, int m, int *need);
void addLink(graph *g, int x, int y, int c);
edge *getEdge(int y, int c);
int min(int a, int b);

int main()
{
	char *inputFile = (char *)malloc(21*sizeof(char));
	printf("Enter the name of the input File : ");
	scanf("%s", inputFile);
	graph *g = ReadGraph(inputFile);

	printf("The graph entered is : \n");
	PrintGraph(g);
	printf("\n**********************Max Flow Computation**********************\n\n");
	int s, t;
	printf("Enter the Source and Sink vertex : ");
	scanf("%d %d", &s, &t);//Input of source and sink

	ComputeMaxFlow(g, s, t);//Max Flow s-->t
    printf("\nThe final graph with flows assigned is : \n");
    PrintGraph(g);//Printing the final flow assigned graph
	g = ReadGraph(inputFile);//Reading graph again

	printf("\n******************Need Based Flow Computation*******************\n\n");

	NeedBasedFlow(g);//Need based Max Flow
	
}

graph *ReadGraph(char *fname)
{
	FILE *fp = fopen(fname, "r");
	int n, m;
	fscanf(fp, "%d %d", &n, &m);//Vertices and edges
	int *need = (int *)malloc((n+1)*sizeof(int));

	for(int i = 1; i<=n; i++)
	{
		fscanf(fp, "%d", &need[i]);
	}//Input of need

	graph *g = getGraph(n, m, need);

	for(int i = 0; i<m; i++)
	{
		int x, y, c;
		fscanf(fp, "%d %d %d", &x, &y, &c);//Input of edges

		addLink(g, x, y, c);
	}
	fclose(fp);
	return g;
}

graph *getGraph(int n, int m, int *need)
{
	graph *g = (graph *)malloc(sizeof(graph));
	g->v = n;
	g->e = m;

	g->h = (vertex *)malloc((n+2)*sizeof(vertex ));

	for(int i = 0; i<=n+1; i++)
	{
		g->h[i].x = i;
		g->h[i].n = need[i];
		g->h[i].p = (edge *)malloc(sizeof(edge));
		g->h[i].p = NULL;
	}

	return g;
}/*Function to initialise an empty graph*/

void addLink(graph *g, int x, int y, int c)
{
	edge *p = getEdge(y, c);
	p->next = g->h[x].p;
	g->h[x].p = p;
}/*Add an edge to the graph*/

edge *getEdge(int y, int c)
{
	edge *new = (edge *)malloc(sizeof(edge));

	new->y = y;
	new->c = c;
	new->f = 0;

	new->next = (edge *)malloc(sizeof(edge));
	new->next = NULL;

	return new;
}/*Get a new edge*/

void PrintGraph(graph *g)
{
	int n = g->v;

	for(int i=1; i<=n; i++)
	{
		printf("%d ", i);
		edge *e = g->h[i].p;
		while(e!=NULL)
		{
		    if(e->y!=0&&e->y!=(n+1))
			    printf("-> (%d, %d, %d) ", e->y, e->c, e->f);
			e = e->next;
		}

		printf("\n");
	}
}/*Function to print the graph*/

void NeedBasedFlow(graph *g)
{
	int n = g->v;
	int pos = 0, neg = 0;
	/*The vertex is a producer if need<0 and consumer if need>0
	So we take 2 extra vertices 0 and n+1. Connect 0 to all the producers
	and n+1 to all the consumers. The 0 becomes the source and n+1 becoes
	the sink in subsequent maxFlow computation*/
	for(int i = 1; i<=n; i++)
	{
	    int need = g->h[i].n;
		if(need<0)
		{
			neg = neg - need;
			addLink(g, 0, i, -1*need);
		}
		else if(need>0)
		{
			pos = pos + need;
			addLink(g, i, n+1, need);
		}
	}
	if(pos!=neg)//Total flow produced necessarily= Total flow consumed
	{
		printf("Need based flow assignment meeting the constraints not possible\n");
	}
	else
	{
		int maxFlow = 0;
		ComputeMaxFlow(g, 0, n+1);
		edge *e = g->h[0].p;
		while(e!=NULL)
		{
			maxFlow += e->f;

			e = e->next;
		}
		if(maxFlow==pos)//Condition for feasibility of flow assignment as per constraints
		{
			printf("Need based flow assignment meeting the constraints possible\n");
	    	printf("The final need based flow assigned graph is : \n");
	    	PrintGraph(g);
		}
		else 
		{
		    printf("Need based flow assignment meeting the constraints not possible\n");
		}
	}

}

void ComputeMaxFlow(graph *g, int s, int t)
{
    int add = 1;
    int maxFlow = 0;
    while(add!=0)//While augmented flow != 0
    {
		graph *resG = computeResidualGraph(g);
		add = augmentFlow(g, resG, s, t);
		maxFlow += add;
    }		
  	
  	if(s!=0)
    	printf("The maximum flow from %d to %d is: %d\n", s, t, maxFlow);
    else
    	printf("The need based flow in the graph is: %d\n", maxFlow);
}

graph * computeResidualGraph(graph *g)
{
	/*Initialising empty residual graph*/
	graph *resG = (graph *)malloc(sizeof(graph));
	resG->v = g->v;
	int n = g->v;
	
	resG->h = (vertex *)malloc((n+2)*sizeof(vertex ));

	for(int i = 0; i<=n+1; i++)
	{
		resG->h[i].x = i;
		resG->h[i].p = (edge *)malloc(sizeof(edge));
		resG->h[i].p = NULL;
	}

	for(int i = 0; i<=n+1; i++)
	{
		edge *e = g->h[i].p;
		while(e!=NULL)
		{
			int source = i;
			int dest = e->y;
			int cap = e->c;
			int flow = e->f;

			if(flow>0)//If current flow>0 add an edge from dest->source of capacity flow
			{
				addLinkResGraph(resG, dest, source, flow);
				if(flow<cap)
					addLinkResGraph(resG, source, dest, cap-flow);
					//If flow<capacity add an edge dest->source of capacity cap-flow
			}
			else
			{//If flow = 0 simply replicate the edge as in original graph
				addLinkResGraph(resG, source, dest, cap);
			}

			e = e->next;
		}
	}

	return resG;
}

void addLinkResGraph(graph *g, int x, int y, int c)
{
	/*If edge x to y exists, increase its cap by c
	  If edge x to y does not exist, add a new edge x->y of cap c*/

	edge *e = g->h[x].p;
	int exists = 0;

	while(e!=NULL)
	{
		int end = e->y;
		if(end==y)//Edge exists, increase capacity of existing edge
		{
			exists = 1;
			e->c = e->c + c;
			break;
		}

		e = e->next;
	}

	if(!exists)//Edge does not exist, add a new edge
	{
		edge *p = getEdge(y, c);
		p->next = g->h[x].p;
		g->h[x].p = p;
	}
}

int augmentFlow(graph *g, graph *resG, int s, int t)
{
	int n = g->v;
	int *dist = (int *)malloc((n+2)*sizeof(int ));
	int *parent = (int *)malloc((n+2)*sizeof(int ));
	int *colour = (int *)malloc((n+2)*sizeof(int ));
	int *minCap = (int *)malloc((n+2)*sizeof(int ));
	for(int i=0; i<=n+1; i++)
	{
		minCap[i] = 0;
		dist[i] = n+3;
		parent[i] = -1;
		colour[i] = 0;//0->Unvisited, 1->in Queue, 2->Popped from queue
	}
	int *queue = (int *)malloc((n+2)*sizeof(int ));
	int start = -1, end = -1;
	queue[0] = s;
	start = 0;
	end = 0;
	dist[s] = 0;
	parent[s] = -1;
	minCap[s] = INT_MAX;
	//Modified BFS to find the required augmenting path
	while(start<=end)
	{
		int source = queue[start%(n+2)];
		start++;
		colour[source] = 2;
		edge *e = resG->h[source].p;

		while(e!=NULL)
		{
			int dest = e->y;
			if(colour[dest]==0)
			{
				queue[(end+1)%(n+2)] = dest;
				end++;
				colour[dest] = 1;
				dist[dest] = 1 + dist[source];
				parent[dest] = source;
				minCap[dest] = min(minCap[source], e->c);
			}
			else if(colour[dest]==1)
			{
				if(dist[dest]==(1+dist[source]))
				{
					if(minCap[dest]<min(minCap[source], e->c))
					{
						minCap[dest] = min(minCap[source], e->c);
						parent[dest] = source;
					}
				}
			}
            
			e = e->next;
		}
		
		if(dist[source]>dist[t])//Already reached a level below the level of sink
		    break;
	}

	int augFlow = minCap[t];//augmenting flow

	int child = t;
	int par = parent[child];
	while(par!=-1)//Adding the augmenting flows to the edges of the augmenting path
	{
	    int p = parent[1];
		int remFlow = augFlow;
		edge *e = g->h[par].p;
		int count = 0;
		while(e!=NULL)//First trying to push the maximum possible flow along the source->dest edge
		{
			int dest = e->y;
			if(dest==child)
			{
				int currFlow = e->f;
				int capacity = e->c;

				e->f = e->f + min(remFlow, capacity-currFlow);
				remFlow = remFlow - min(remFlow, capacity-currFlow);

				break;
			}
            count++;
            
			e = e->next;
		}

		edge *e2 = g->h[child].p;
		while(e2!=NULL)//If not enough remaining capacity, push negative flow along dest->source (effectively flow along sourcce->dest)
		{
			int dest = e2->y;
			if(dest==par)
			{
				e2->f = e2->f - remFlow;
				break;
			}
			
			e2 = e2->next;
		}

		child = par;
		par = parent[child];
	}

	return augFlow;
}

int min(int a, int b)//Function to find minimum of 2 integers
{
    return ((a<b)?a:b);
}