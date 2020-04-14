/**********************************************************************************
*	FILENAME    : server.h
*
*   DESCRIPTION : File contains headers of server that accepts the request from 
*			      the clients and provides services as per their requests
*
*	AUTHOR		 : GR_McCARTHY_CPP_06
*	
*	DATE		 : 30 JANUARY,2020				
*	
*   COPYRIGHT  @ALTRAN.COM
*
***********************************************************************************/

/************************************************************************************
							GUARD FUNCTIONS
*************************************************************************************/
#ifndef __SERVER_H_
#define __SERVER_H_

/************************************************************************************
							STANDARD HEADER FILES
*************************************************************************************/
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

/************************************************************************************
								MACROS
*************************************************************************************/

#define BUFFERSIZE 1024		// Max Buffer Size

using namespace std;

/************************************************************************************
								 CLASS DECALARATION
*************************************************************************************/

class Server {
	
	public:
		Server();	// Default Constructor
		~Server();	// Destructor
	
		// Creation of a Socket
		int OpenSocket();
	
		// Bind Data
		int BindData();
		
		// Listen for Connections
		int Listen();
		
		// Server gets a Socket for an Incoming Client Connection
		int Accept();
				
		// Read Data sent by Client
		int ReadData();

		// Send Data to Client over the Socket
		int SendData(char* data);
		
		// Tokenize Data sent by Client
		int TokenizeData(char* data);
		
		// Search if Mobile Number exists or not
		int SearchMobNum(string data, string mNum);
		
		// Check if Mobile Number is Registered or Not
		int IsNumberRegistered();
		
		// Check if Service is Activated for Mobile Number
		int IsServiceActivated();
		
		// To Process Activation Request for Global Service
		int ProcessGlobalActivationRequest();
		
		// To Process Activation Request for Selective Service
		int ProcessSelectiveActivationRequest(string blockNum);
		
		// To Process Deactivation Request for Mobile Number
		int ProcessDeactivationRequest();
		
		// Process Update Service Request for Mobile Number
		int	ProcessUpdateRequest();
		
		// Process Call to another Client
		int ProcessCallRequest(string callNum);
		
		// Write Data to File
		int WriteFile(char* data, char* fname);
	
	private:
		int sd;		// Socket Descriptor
		int cd;		// Connection Descriptor
		int fd;     // File Descriptor
		string mobNumber;	// Mobile Number sent by Client
		char databuf[BUFFERSIZE];	// Store Data sent by Client
		vector<string> clientRecord;	// Store Each Client Record
		char clientIpAddress[BUFFERSIZE];   // Store Client IP Address 
		struct sockaddr_in serverAddress;	// Server Address
		struct sockaddr_in clientAddress;	// Client Address
	
};

void signal_handler(int sig);

#endif
