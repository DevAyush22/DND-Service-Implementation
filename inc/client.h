/**********************************************************************************
*	FILENAME    : client.h
*
*   DESCRIPTION : File contains headers of client that sends the request to the server
*					and provides services as per the requests
*
*	AUTHOR		: GR_McCARTHY_CPP_06
*	
*	DATE		: 30 JANUARY,2020				
*	
*   COPYRIGHT  @ALTRAN.COM
***********************************************************************************/

/************************************************************************************
							GUARD FUNCTIONS
*************************************************************************************/

#ifndef __CLIENT_H_
#define __CLIENT_H_

/************************************************************************************
							STANDARD HEADER FILES
*************************************************************************************/

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

// Check if given string is a numeric string or not
bool IsNumber(const string & str);

// Validate IP Address
bool IsValidIp(string ip);

// Validate Port Number
bool IsValidPortNum(string portNum);

// Validate Mobile Number
bool IsValidMobileNum(string mobNum);

// Signal Handling
void signal_handler(int sig);


/***************************************************************************************************
							CLASS DECLARATION
****************************************************************************************************/
class Client {
	
	public:
		Client();	// Default Constructor
		Client(string IpAddress, string PortNo, string mobNum);	// Parameterized Constructor
		~Client();	// Destructor
		
		// Creation of a Socket
		int OpenSocket();	
		
		// Connecting to the Server
		int Connect();
		
		// Checking if the Mobile Number is Registered
		int IsMobNumRegistered(string mobNum);
		
		//Displaying the Main Menu
		int MainMenu();
	
		// Sending Data to Server
		int SendData(char* data);
		
		// Read the Data sent by Server
		int ReadData();
		
		// Check if Service is Activated for Mobile Number
		int IsServiceActivated();
		
		// Activate Service
		int ActivateService();
		
		// Deactivate Service
		int DeactivateService();
		
		// Update Service
		int UpdateService();
		
		// Call another Client
		int CallClient();
		
	private:
		int sd;		// Socket Descriptor
		struct sockaddr_in serverAddress;	// Server Address
		string mobileNumber;	// Mobile Number

};

#endif
