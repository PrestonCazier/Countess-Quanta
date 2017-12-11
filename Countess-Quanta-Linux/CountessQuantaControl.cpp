#include "ros/ros.h"
#include "std_msgs/String.h"

bool newCommandReceived = false;

void chatterCallback(const std_msgs::String& msg)
{
	// handle input from RealSense speech message here
	new commandRecieved = true;
}

int main(int argc, char** argv)
{
 	ros::init(argc, argv, "listener");
	ros::NodeHandle nodeHandle;
	ros::Subscriber subscriber = nodeHandle.subscribe("chatter",10, chatterCallback);
	ros::Publisher chatterPublisher = nh.advertise<std_msgs::String>("chatter", 1);
	ros::Rate loopRate(10);
	unsigned int count = 0;
	while(ros::ok()) 
	{
		std_msgs::String message;
		message.data = "new message from RealSense was recievedn\n";
		ROS_INFO_STREAM(message.data);
		chatterPublisher.publish(message);
		ros::spinOnce();
		loopRate.sleep();
		count++;
	}
	return 0;
}
