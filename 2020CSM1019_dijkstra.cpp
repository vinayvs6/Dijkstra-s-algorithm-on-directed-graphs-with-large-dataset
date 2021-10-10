//Vinay & 2020CSM1019

#include <iostream>
#include <limits.h>
#include <unordered_map>
#include <vector>
#include <sstream>
using namespace std;

struct node	//structure for storing nodeIDs and maintaining its distances
{
	long NodeID;
	int distance;
};

//The below are the all functions that i have used in this program
void dijkstra(long src, long dest);
void buildMinHeap();
void minHeapify(long i);
long extractMin();
void reduceLength(vector<node>& heapnodes, int new_length, long loct);
void edgeReaxation(long head_node, long tail_node, long length);
void displayShortestpath(long dest);

vector<node> nodes;	//This is to store the nodes
unordered_map<long, long> nodeloct, pathnodes, visited;	//
unordered_map<long, vector<pair<long, int>>> edges;
int loct = 0;

long heapsize;
vector<node> heapnodes;
unordered_map<long, long> heapnodeloct;

int main()
{
	nodes.push_back(node{0,0});

	FILE* nf = fopen("nodes.txt","r");	//file pointer for the nodes file
	FILE* ef = fopen("edges.txt","r");	//file pointer for the edges file

	if(nf==NULL)	//if no nodes file
	{
		cout<<"nodes file doesn't exist\n";
		return 0;
	}

	if(ef==NULL)	//if no edges file exist
	{
		cout<<"edges file doesn't exist";
		return 0;
	}

	long src, dest;
	cout<<"\nEnter the source Node ID: "; cin>>src;
	cout<<"\nEnter the destination Node ID: "; cin>>dest;
	
	char buffer[100];

	while(fgets(buffer, 100, nf))	//Storing all the nodes from the nodes file of graph by fetching each line
	{
		long NodeID = atol(buffer);		//converting the retrieved line i.e, a string format to a number by using atol function
		nodes.push_back(node{NodeID, INT_MAX});	//Intially each node's distance storing with maximum value
		nodeloct[NodeID] = ++loct;			//indexing each node in another map
		pathnodes[NodeID] = -1;				//to maintain path nodes and initializing with -1
		edges[NodeID] = {};					//to maintain edges
		visited[NodeID] = 0;				//to maintain the nodes are visited or not
	}

	if(nodeloct[src] == 0)		//if the source node is not exist, then this condition will satisfy
	{
		cout<<"Source Node ID not exist\n";
		return 0;
	} 
		
	else nodes[nodeloct[src]].distance = 0;	//otherwise make the source node distance as 0

	if(nodeloct[dest] == 0)	// same as above if condition, but here it's for destination node
	{
		cout<<"Destination Node ID not exist\n";
		return 0;
	}


	fclose(nf);

	long head_node, tail_node;
	int i, edgeWGT;

	while(fgets(buffer, 100, ef))	//retrieving each line from the edges file
	{
		stringstream commasep(buffer);	//as there is comma separated string in each line 
		vector<string> v;	//to store as 3 tokens in the vector

		while (commasep.good())
		{
        string token;
        getline(commasep, token, ',');
        v.push_back(token);
    	}

    	head_node = stol(v[0]);	//then assigning the two node IDs with its edge weight between them
    	tail_node = stol(v[1]);
    	edgeWGT = stoi(v[2]);

		edges[head_node].push_back(make_pair(tail_node, edgeWGT));	//Inserting adjacent nodes into the map with its edge weight
	}

	fclose(ef);


	dijkstra(src, dest);

	return 0;
}


void dijkstra(long src, long dest)	//this is the dijkstra algo for the program to find shortest path between two nodes
{
	heapsize = nodes.size()-1;	//initializing heap size equal to number of nodes
	heapnodes = nodes;			//to get the heapified version of nodes
	heapnodeloct = nodeloct;	//To maintain the heapified nodes' locations

	buildMinHeap();	//calling buildMinHeap to build the minheap

	long nodenear;

	while(heapsize != 0)	//this loop will execute unil there is no node in the heap
	{
		nodenear = extractMin();	//minimum node will give nearest node to the head/source node
		visited[nodenear] = 1;	//this is like a flag value which show if the nearest node is already visited or not

		for(auto &it : edges[nodenear])	//this loop is to fetch all adjacent nodes
		{
			if(!visited[it.first])		//if the adjacent node is not visited already, we need to consider that node
				edgeReaxation(nodenear, it.first, it.second);	//and checks if there is better length we can find out through that node
		}

	}

	if(nodes[nodeloct[dest]].distance == INT_MAX)	//if the destination node INT_MAX value only means the initial distance not changed. so there's no path between source&destination
	{
		cout<<"\nNo path exists between "<<src<<" & "<<dest<<"!"<<endl;;
	}
	else	//else we can print the shortest path between source & destination
	{
		cout<<"\nThe path between "<<src<<" & "<<dest<<" is: \n\n";
		displayShortestpath(dest);
		cout<<"\b\b\b   \n\nthe length of the path between "<<src<<" & "<<dest<<" is: "<<nodes[nodeloct[dest]].distance<<endl<<endl;
	}

}

void buildMinHeap()	//it's a normal function to start building min heap
{
	for(long i = heapsize/2 ; i > 0 ; i--) minHeapify(i); //Initialized the i value with last non-leaf node
}


void minHeapify(long i)	//This function performs minheap operations
{
	long Min_Node = i;		//The non-leaf node index we got from function buildMinHeap()
	long left = 2*i;		//left child of non-leaf node
    long right = 2*i+1;		//left right of non-leaf node
	
	if(left <= heapsize && heapnodes[left].distance < heapnodes[i].distance)	//If the node is greater than its left child
	    Min_Node = left;

	if(right <= heapsize && heapnodes[right].distance < heapnodes[Min_Node].distance)	//If the node is greater than its right child
	    Min_Node = right;

	if(Min_Node != i)	//If Min_node changes its value then, we again need to check with its child nodes after swapping current node with new minimum node
	{
		node temp = heapnodes[i];
		heapnodes[i] = heapnodes[Min_Node];
		heapnodes[Min_Node] = temp;

		long tt = heapnodeloct[heapnodes[i].NodeID];
		heapnodeloct[heapnodes[i].NodeID] = heapnodeloct[heapnodes[Min_Node].NodeID];
		heapnodeloct[heapnodes[Min_Node].NodeID] = tt;

		minHeapify(Min_Node);
    }

}

long extractMin()	//it's a standard algo to extract the minimum node from the minheap
{
	long minnodeID = heapnodes[1].NodeID;	//first node is the minimum node in the minheap
	heapnodes[1] = heapnodes[heapsize--];	//swapping first node with last last node
	heapnodeloct[heapnodes[1].NodeID] = 1;	//and their locations also
	minHeapify(1);	//after extracting the minimum node we again need to heapify the minheap

	return minnodeID;	//returning the minimum node
}


void edgeReaxation(long head_node, long tail_node, long length)	//edge realaxation function is nothing a process of checking if there is an edge for finding better path
{
	long src = nodes[nodeloct[head_node]].distance;		//taking head node distance as source distance
	long dest = nodes[nodeloct[tail_node]].distance;	//and tail node distance as destination distance
	if( dest > src + length )			//if the tail node distance is greater than the newly calculated distance through a head node 
	{									//then, we need to consider the new edge between the head to tail nodes
		pathnodes[tail_node] = nodeloct[head_node];	//update the new node into paths
		reduceLength(heapnodes, src + length ,heapnodeloct[tail_node]);	//then the destination node length should be decremented
		nodes[nodeloct[tail_node]].distance = src + length;			//now the new length/distance should be upated
	}
}

void reduceLength(vector<node>& heapnodes, int new_length, long loct)	//this function to update the decremented length 
{
	heapnodes[loct].distance = new_length;	//updating new length to the tail node location that we got from edgerelaxation function

	while(loct>0 && heapnodes[loct/2].distance > heapnodes[loct].distance)	//if child node distance have less distance than parent node, we have to swap both nodes 
																				//and repeat until all parent nodes have less distance than its childs
	{
		node temp = heapnodes[loct];
		heapnodes[loct] = heapnodes[loct/2];
		heapnodes[loct/2] = temp;

		long tt = heapnodeloct[heapnodes[loct].NodeID];
		heapnodeloct[heapnodes[loct].NodeID] = heapnodeloct[heapnodes[loct/2].NodeID];
		heapnodeloct[heapnodes[loct/2].NodeID] = tt;

		loct/=2;
	}
}

void displayShortestpath(long dest)
{
	if(pathnodes[dest] == -1)
	{
		cout<<dest<<" -> ";
		return;
	}
	displayShortestpath(nodes[pathnodes[dest]].NodeID);
	cout<<dest<<" -> ";
}