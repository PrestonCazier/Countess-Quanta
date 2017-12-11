// Michael Lowe, ECE 478, PSU Fall 2011
//Graph.cpp is the method definitions for the graph class

#include <iostream>
#include "graph.h"
#include <string>


using namespace std;

#define debug 0

//Default Destructor
Graph::Graph()
{
}



//Constructor that takes an array of strings and creates the beging of an array of linked lists
//IN: The number of Locations that will be in and the array of Locations in the string
Graph::Graph(int locationNum, string Locations[])
{
	LocationsAndRoutes = new location * [locationNum];
	//currentLocation = NULL;
	head= new location[locationNum]; 
	for(int i=0; i<=locationNum-1;i++)
	{
		head[i].name = Locations[i];
		head[i].next=NULL;
		head[i].index=i;
		LocationsAndRoutes[i] = &head[i];
		if (debug) cout<<head[i].name<<" "<<head[i].index<<endl;
	}
	
	NumOfLocations = locationNum;
}



//destructor
Graph::~Graph()
{
	for(int i= 0; i<=NumOfLocations-1; i++)
	{
	location * pre = LocationsAndRoutes[i];
	location * cur;

		while(pre->next != NULL && pre->next->index >= 0)
		{
			cur = pre->next;
			pre->next = cur->next;	
			delete cur;
			cur=NULL;

		}
		pre->next=NULL;
		
    }
	
	delete [] head;
	delete [] LocationsAndRoutes;
	
}


//method for setting the current location
void Graph::setCurrentLocation(string curLoc)
{
	currentLocation = curLoc;
}

//method for getting the current location string
string Graph::getCurrentLocation()
{
	return currentLocation;
}


//This method adds routes from one location to antother
//The routes are directed
//IN:The location the robot is coming from and the location the robot is going to
bool Graph::addEdges(string location1, string location2)
{
	//The default indexes of the strings, If they are not set an error will be returned
	int locationFrom = -1;		
	int locationTo = -1;		
	//pointers for the location that the robot is going from and to
	location * cur;
	location * goTo=new location;

	//finds the location indexes
	locationFrom = getIndex(location1);
	locationTo = getIndex(location2);

	//If one of the values was not set(no match) return false
	if(locationTo == -1 || locationFrom == -1)
		return false;

	cur = LocationsAndRoutes[locationFrom];

	//sets location with values of the location that the robot goes to
	goTo->next=NULL;
	goTo->index=locationTo;
	goTo->name = location2;

	//move to the last node of the location coming from
	while(cur->next != NULL)
		cur=cur->next;
	cur->next = goTo;

	return true;
}



//This method finds if there is a route from one location to another, directly or indirectly
//IN:The location the robot is coming from and the location the robot is going to
stack<location> Graph::findPath(string locationFrom, string locationTo)
{
	//The default indexes of the strings, If they are not set an error will be returned
	int location1 = -1;		
	int location2 = -1;		
		if(debug) cout<<"1"<<endl;
	//array to mark visited locations
	int * visited = new int[NumOfLocations];
	for(int i=0; i<=NumOfLocations-1; i++)
		visited[i]=0;
		if(debug) cout<<"2"<<endl;
	//pointers for the locat4ion that the robot is going from and to
	location * cur;
	location * goTo;
	if(debug) cout<<"3"<<endl;
	//stack to keep track of the current location
	stack<location> locationStack;
	if(debug) cout<<"4"<<endl;
	//Loop to find matches for the location the robot is going from and to
	if (debug) cout<<locationFrom<<endl;
	if (debug) cout<<locationTo<<endl;
	location1= getIndex(locationFrom);
	location2= getIndex(locationTo);
		if(debug) cout<<"loc1 " <<location1<<endl;
		if(debug) cout<<"loc2 " <<location2<<endl;
	cur = LocationsAndRoutes[location1];
	goTo = LocationsAndRoutes[location2];
	if(debug) cout<<"4"<<endl;
	//If one of the values was not set(no match) return false
	//if(location2 == -1 || location1 == -1)
	//	return false;

	locationStack.push(*cur);
	visited[cur->index]=1; 

	while(!locationStack.empty())
	{
		if(cur->name.compare(goTo->name)==0)
		{
			delete [] visited;
			return locationStack;// true;					//returns true if there is a route
		}
		while(cur->next!=NULL && visited[cur->next->index]==1)
					cur=cur->next;
	
		if(cur->next == NULL)
		{
			locationStack.pop();
			if(locationStack.empty())
				//return false;		//return false if the stack is empty after pop
			*cur=locationStack.top();
			cur=LocationsAndRoutes[cur->index];
		}

		else
		{
			cur=LocationsAndRoutes[cur->next->index];
			locationStack.push(*cur);
			visited[cur->index]=1;
		}

	}
	delete [] visited;
	
}



//procedure used to find the index of the location
//IN:the location whose index is to be gotten
int Graph::getIndex(string locationToGet)
{	
	
	location * cur;
	for(int i = 0; i<=NumOfLocations-1 ; i++)
	{
		cur = LocationsAndRoutes[i];
		if(debug) cout<<cur->name<<endl;
		if(locationToGet.compare(cur->name)==0)
			return i;				//return index if a match
	}

	return -1; //return invalid index if no match
}
