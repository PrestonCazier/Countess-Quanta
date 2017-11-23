/*
Program to check if bot is working
Author: sreeram
*/
#include <iostream>
#include <graph.h>
#include <mrpt/hwdrivers/CActivMediaRobotBase.h>
#include <mrpt/base.h>
#include <mrpt/slam/CSensoryFrame.h>
#include <unistd.h>

#define ROBOTICS_LAB 0
#define MENS_ROOM 1
#define LADIES_ROOM 3
#define GLASS_DOOR 2
#define TEKTRONIX 4
#define V_LAB 5

using namespace mrpt::hwdrivers;
using namespace mrpt::utils;
using namespace mrpt::math;
using namespace mrpt::slam;
using namespace mrpt::poses;
using namespace std;

static void moveForward(double distance);
//void moveBackward(CActivMediaRobotBase robot, double distance);
static void turn(double degRadians);
int getCurLocation();
double avoidObstacle(double speed);
double getSonarDist(int ID);
Graph setupGraph();

void moveFrom0To1();
void moveFrom1To2();
void moveFrom2To3();
void moveFrom2To5();
void moveFrom3To4();
void moveFrom1To0();
void moveFrom2To1();
void moveFrom3To2();
void moveFrom5To2();
void moveFrom4To3();

string travelToLocation(stack<location> path,string destination, string currentLocation);


//Robot object
CActivMediaRobotBase	robot;
CSensoryFrame frame1;
//index for current location
int 			curLocation;


//**MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN****MAIN**
int main(int argc, char **argv)
{

//Set Communication port up*************************************************************************************
	try
	{
		
#ifdef MRPT_OS_WINDOWS
		string	port="COM4";
#else
		string	port="/dev/ttyUSB0";
#endif

		int 	port_baud = 9600;

		cout << "Usage: " << argv[0] << " [SERIAL_PORT [BAUD_RATE] ]" << endl;

        if (argc>=2)
        {
            port = std::string(argv[1]);

            if (argc>=3)
                port_baud = atoi(argv[2]);
        }

		cout << "Setting serial port to: " << port << " @ " << port_baud << endl;
		robot.setSerialPortConfig( port, port_baud );


		// -------------------------------------------
		//  Init comms:
		// -------------------------------------------
		robot.enableSonars();
		robot.initialize();
		
		robot.enableJoystickControl();

//*****************************************************************************************


		

//Initialize Robot and check sensors*********************************************************
		CActivMediaRobotBase::TRobotDescription  robInfo;
		robot.getRobotInformation(robInfo);

		cout << "Robot # front bumpers : " << robInfo.nFrontBumpers << endl;
		cout << "Robot # rear bumpers  : " << robInfo.nRearBumpers << endl;
		cout << "Robot # sonars        : " << robInfo.nSonars << endl;
		

		bool show_menu = true;
		bool run = true;
		
//*******************************************************************************************

			
			

	
 	char cityGo;		//checks to see if the user will enter another city
	string places[255];		//next city to be entered in the map
	int count = 0;			//keep count of the number of cities

	cout<<"Creating Graph..."<<endl;
	

	//create the graph by adding locations to place array
	places[0] = "Robotics Lab";
	places[1] = "Mens Room";
	places[2] = "Glass Door";
	places[3] = "Ladies Room";
	places[4] = "Tektronix";
	places[5] = "V Lab";

	Graph map(6,places);

	cout<<endl<<"Locations added to graph!!!"
		<<endl<<"Now creating edges..."<<endl;

	//variables to initialize edges

	bool check;		//make sure method call is successful

	//initialize edges
	check = map.addEdges(places[0],places[1]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[1],places[2]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[2],places[3]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[2],places[5]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[3],places[4]);
	if (!check) cout<<"An edge was not added!!!"<<endl;

	check = map.addEdges(places[1],places[0]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[2],places[1]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[3],places[2]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[5],places[2]);
	if (!check) cout<<"An edge was not added!!!"<<endl;
	check = map.addEdges(places[4],places[3]);
	if (!check) cout<<"An edge was not added!!!"<<endl;

	cout<<"Done adding edges"<<endl;


	//Start at the robotics lab
	map.setCurrentLocation("Robotics Lab");
	char locGo = 'Y'; 	//Keep moving until done
	string destination;	//location robot will be going to  
        stack<location> path;	//stack of the paths that robot must traverse
	while (locGo=='Y')
	{
		
		cout<<"Current Location is " << map.getCurrentLocation()<<endl;
		cout<<"Where would you like to go?"<<endl;
		getline(cin,destination);
		
		//get a path for the robot to follow
		path = map.findPath(map.getCurrentLocation(),destination);
		//travel to the location and return the location the robot made it to
		destination = travelToLocation(path,destination,map.getCurrentLocation()); 
		//set location to the location traveled to at previous step
		map.setCurrentLocation(destination);
		cout<<"Would you like to enter another location(Y/N)"<<endl<<endl;
		cin>>locGo;
		locGo = toupper(locGo);
		cin.ignore();
		
	}
		cout << "program done \n";
	}
	catch(std::exception &e)
	{
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
//**END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN****END_MAIN**





//Functionn to move the robot forward (if negative robot will not move)
//IN:  Distance to travel forward in meters
static void moveForward( double distance)
{
	
	
	double speed = 0.5;	//speed to set robot to in meters/second
	bool drive = true;
	

	//set velocity to get robot up to speed
	robot.setVelocities(0.25,0);
	sleep(200);

	//set velocity to driving speed
	robot.setVelocities(speed,0);
	

	while(drive)
	{
		//check if something is in front ever 100ms
		if (getSonarDist(3) < 0.4 || getSonarDist(4) < 0.4)
		{
			distance = distance - avoidObstacle(speed);  //subtract the distance traveled during avoid
		}
		distance = distance - (speed/10);		//ever 100ms subtract 1/10 the speed
		sleep(100);	
		if(distance <=0) drive = false;
	}
	//stop robot
	robot.setVelocities(0,0);
}


/*Method to turn the robot
 *IN: Distance to turn in radians
 */
static void turn( double degRadians)
{

	double speed;
	//check if a negative input
	if(degRadians > 0) speed = M_PI/10;
	if(degRadians < 0) 
	{
		speed = -M_PI/10;
		degRadians = degRadians * -1;
	}
	bool turn = true;
	robot.setVelocities(0,speed);
	
	//compensate for startup
	sleep(100);

	//turn until desired position is reached, check every .1 seconds
	while(turn)
	{
		degRadians = degRadians - fabs(speed/10);
		sleep(100);
		if(degRadians <= 0) turn = false;
		
	}
	robot.setVelocities(0,0);
	
}


//Find out where the robot is currently at
//Currently not used
int getCurLocation()
{
	return ROBOTICS_LAB;
}


//Method to avoid obsticle
//Not optimal with path avoidance... Improve later
double avoidObstacle(double speed)
{
	bool forward = true;	//drive forward boolean	
	double disTraveled1 = 0;	//keep track of distance traveled to return
	double disTraveled2 = 0;
	double disTraveled3 = 0;

	//stop when object is sensed
	robot.setVelocities(0,0);
	
	//turn at a 45 degree angle then go forward until back side sensor sees object
	turn(M_PI/4);
	//set to .2 m/s ... slowly avoid
	robot.setVelocities(0.2,0);
	int j =0;
	//go forward using side sensors to know when to pass
	while(forward)
	{
		j++;
		if (getSonarDist(8) > .5 && getSonarDist(7) > 0.5 && j>15) forward = false;
		disTraveled1 = disTraveled1 + .02/sqrt(2);
		sleep(100);
	}
	disTraveled3 = disTraveled1;
	cout<<"stage1"<<endl;
	robot.setVelocities(0,0);
	forward = true;

	//turn back 45 degrees so robot is going straight again
	turn(-M_PI/4);
	robot.setVelocities(0.2,0);
	int i = 0;
	while(forward)
	{
		i++;
		if (getSonarDist(8) > .4 && getSonarDist(7) > .4 && i >15) forward = false;
		disTraveled2 = disTraveled2 + .02;
		sleep(100);
	}
	
	//turn back to head to previous line of travel
	cout<<"stage2"<<endl;
	robot.setVelocities(0,0);
	forward = true;
	turn(-M_PI/4);
	robot.setVelocities(0.2,0);
	while(forward)
	{
		sleep(100);
		disTraveled3 = disTraveled3 - 0.02/sqrt(2);
		if (disTraveled3 <= 0 ) forward = false;
	}
	robot.setVelocities(0,0);
	//last turn to get back on path
	turn(M_PI/4);
	robot.setVelocities(speed,0);	
	return ((2*disTraveled1) + disTraveled2);
}

//easy call to method to get info from certain sensor
double getSonarDist(int ID)
{
	CObservationRange obs;
	bool thereis;
	robot.getSonarsReadings(thereis,obs);
	//cout<< obs.sensedData[ID].sensorID<< ": "<<obs.sensedData[ID].sensedDistance<<endl;
	return obs.sensedData[ID].sensedDistance;
}

//Move From the robotics lab to the Mens Room
void moveFrom0To1()
{
	turn(-(M_PI/2));
	moveForward(4.9);
	turn(-(M_PI/2));
	moveForward(6.8);
	
}

//Move from the mens room to the robotics lab
void moveFrom1To2()
{
	turn(-(M_PI/2));
	moveForward(2.4);
	turn(M_PI/2);
	moveForward(11.6);
}

//Move from glass door to Ladies room
void moveFrom2To3()
{
	turn(M_PI/2);
	moveForward(2.4);
	turn(M_PI/2);
}

//move from Glass Doors to VLab
void moveFrom2To5()
{
	turn(-(M_PI)/2);
	moveForward(37.8);
	turn(M_PI/2);
}

//move from Ladies room to Tektronix
void moveFrom3To4()
{
	turn(-(M_PI/2));
	moveForward(12.2);
	turn(-(M_PI/2));
}

//Move from the Mens Room to the Robotics Lab
void moveFrom1To0()
{
	turn(M_PI);
	moveForward(6.8);
	turn(M_PI/2);
	moveForward(4.9);
	turn(M_PI/2);
}

//Move from the glass door to the mens room
void moveFrom2To1()
{
	turn(M_PI);
	moveForward(11.6);
	turn(-(M_PI/2));
	moveForward(2.4);
	turn(-(M_PI/2));
}

//move from ladies room to glass door
void moveFrom3To2()
{
	turn(M_PI/2);
	moveForward(2.4);
	turn(M_PI/2);
}

//move from VLab to glass doors
void moveFrom5To2()
{
	turn(M_PI/2);
	moveForward(37.8);
	turn(-(M_PI)/2);
}

//move from tektronix to ladies room
void moveFrom4To3()
{
	turn(-(M_PI/2));
	moveForward(12.2);
	turn(-(M_PI/2));
}

//****************************************************************************************************************************************************
//Having problems with this function, so don't use unless fixed
//setup is implemented in main function
Graph setupGraph()
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		char cityGo = 'Y';		//checks to see if the user will enter another city
	string places[255];		//next city to be entered in the map
	int count = 0;			//keep count of the number of cities

	cout<<"Welcome to the route finder, First enter all cities on the map."<<endl;
	


	//create the graph
	places[0] = "Robotics Lab";
	places[1] = "Mens Room";
	places[2] = "Glass Door";
	places[3] = "Ladies Room";
	places[4] = "Tektronix";
	places[5] = "V Lab";

	Graph map(6,places);

	//variables to initialize edges
	string city1;
	string city2;
	cityGo ='Y';
	bool check;		//make sure method call is successful

	//loop to initialize edges
	check = map.addEdges(places[0],places[1]);
	check = map.addEdges(places[1],places[2]);
	check = map.addEdges(places[2],places[3]);
	check = map.addEdges(places[2],places[5]);
	check = map.addEdges(places[3],places[4]);

	check = map.addEdges(places[1],places[0]);
	check = map.addEdges(places[2],places[1]);
	check = map.addEdges(places[3],places[2]);
	check = map.addEdges(places[5],places[2]);
	check = map.addEdges(places[4],places[3]);
	//Start at the robotics lab
	map.setCurrentLocation("Robotics Lab");
	cout<<"current location set to "<<map.getCurrentLocation()<<endl;
	return map;


	
}
//**************************************************************************************************************************************




//Function to handle travel to locations.
string travelToLocation(stack<location> path, string destination, string currentLocation)
{
	string travelTo;

	/*string destination;
	cout<<"what location is the robot going to?"<<endl;
		getline(cin,destination);
	stack<location> path;*/

	location loc1[255];
	//cout<<"At "<<map.getCurrentLocation()<<endl;
	//cout<<"Going to "<<destination<<endl;
	//path = map.findPath(map.getCurrentLocation(),destination);//map.getCurrentLocation(),destination);
	cout<<"starting loop"<<endl;
		
			int i = 0;
			while(!path.empty())
			{
				loc1[i] = path.top();	
				path.pop();
				cout<<loc1[i].name<<endl;
				i++;
			}
		i--;
		while(i>=0)
		{		
		travelTo = loc1[i].name;
		cout<<"next location is"<<travelTo<<endl;
		if(currentLocation == "Robotics Lab")
		{
			if (loc1[i].name == "Mens Room") moveFrom0To1();

			
		}

		else if(currentLocation == "Mens Room")
		{
			if (loc1[i].name == "Robotics Lab") moveFrom1To0();
			else if(loc1[i].name == "Glass Door") moveFrom1To2();
		}
				


		else if(currentLocation == "Glass Door")
		{
			if (loc1[i].name == "Mens Room") moveFrom2To1();
			else if(loc1[i].name == "Ladies Room") moveFrom2To3();
			else if(loc1[i].name == "V Lab") moveFrom2To5();
		}


		else if(currentLocation == "Ladies Room")
		{
			if (loc1[i].name == "Glass Door") moveFrom3To2();
			else if (loc1[i].name == "Tektronix") moveFrom3To4();
		}
	
		else if(currentLocation == "Tektronix")
		{
			if (loc1[i].name == "Ladies Room") moveFrom4To3();
		}

		else if(currentLocation == "V Lab")
		{
			if (loc1[i].name == "Glass Door") moveFrom5To2();
		}

		else return destination;
		
		//Set to the current location
		//map.setCurrentLocation(travelTo);
		cout<<"Currently at Location "<<travelTo<<endl;
		i--;
		}

		cout<<"At Destination"<<endl;
	
	return destination;
}
