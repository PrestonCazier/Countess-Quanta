// Michael Lowe, ECE 478, PSU Fall 2011, PSU Guidebot
// Graph.h is the header file that defines the Graph class

using namespace std;

struct location
{
	string name;
	location * next;
	int index;
};

class Graph
{
public:
	void addLocation();
	void setCurrentLocation(string curLoc);
	bool addEdges(string loc1, string loc2);
	bool findPath(string locFrom, string locTo);
	int getIndex(string locGet);
	Graph();
	Graph(int locNum, string locations[]);
	~Graph();

private:
	location ** LocationsAndRoutes;
	location * head;
	int NumOfLocations;
	string currentLocation;	
};



