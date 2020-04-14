/**********************************************************************************
*       FILENAME    : client.cpp
*       DESCRIPTION : File contains definitions of the of clients that sends message to the server 
*					  as per the client's request
*
*	AUTHOR		 : GR_McCARTHY_CPP_06
*	
*	DATE		 : 30 JANUARY,2020				
*	
*   COPYRIGHT  @ALTRAN.COM
***********************************************************************************/

/**********************************************************************************
                       STANDARD HEADER FILES
***********************************************************************************/
#include <iostream>		//Input output purposes
#include <vector>		//dynamic arrays with the ability to resize itself 
#include <sstream>		//templates and types that enable interoperation between stream buffers and string objects
#include <unistd.h>		//entry point to various constant, type and function declarations
#include <cstdlib>		//for exit,integer arithmetics, searching, sorting and converting
#include <stdio.h>		//Input output opearation, perror
#include <cstring>		//required for string functions
#include <errno.h>		//provies decalaration for errno
#include <string.h>		//contain classes that are used for exception handling
#include <sys/socket.h>	//define the sockaddr structure that includes at least the following members: sa_family_t sa_family Address family
#include <sys/stat.h>	//defines structure functions returned by fstats() and stat
#include <sys/types.h>	//contains a number of basic derived types 
#include <netinet/in.h>	//contains definitions for the internet protocol family
#include <arpa/inet.h>	//define the INET_ADDRSTRLEN and INET6_ADDRSTRLEN macros
#include <fcntl.h>		//use in the oflag value to open() and openat()

/**********************************************************************************
                       INCLUSION HEADER FILES
***********************************************************************************/
#include <client.h>
#include <logger.h>

/**********************************************************************************
                               MACROS
***********************************************************************************/
#define BLOCKSIZE 512		// Block Size
#define BUFFERSIZE 1024		// Max Buffer Size
#define DELIM '.'           // Delimiter for Tokenizing IP Address

#define SUCCESS 0           // Returns 0 for Success
#define FAILURE 1           // Returns 1 for Failure

using namespace std;


/********************************************************************************************************
* FUNCTION NAME : signal_handler
*
* DISCRIPTION : This function sends the signal on presssing Ctrl+C.
*
* ARGUMENTS : int
*
* RETURN TYPE : void
********************************************************************************************************/
void signal_handler(int sig) {
	
    cout << "Ctrl+C Sent: " << endl << "Client EXIT..."<<endl;  
    exit(EXIT_FAILURE);
}


/****************************************************************************************
FUNCTION NAME	: IsNumber() 
       
DESCRIPTION		: This functions checks if all the characters in the string is a digit
	
PARAMETER		: const string & str
	
RETURN 			: bool
*****************************************************************************************/
bool IsNumber(const string & str) {
	
	log(3, INFO, "Entering", "IsNumber");

	log(3, INFO, "Exiting", "IsNumber");
    // std::find_first_not_of searches the string for the first character 
    // that does not match any of the characters specified in its arguments

    return !str.empty () && 
    (str.find_first_not_of ("[0123456789]") == std::string::npos);	
}



/******************************************************************************************
FUNCTION NAME	: IsValidaIp() 
       
DESCRIPTION  	 : This functions validates IP address. It returns true if valid and false if not valid
	
PARAMETER		: string ip
	
RETURN 			: bool
********************************************************************************************/
bool IsValidIp(string ip) {

	log(3, INFO, "Entering", "IsValidIp");
    
    //Split the string into tokens
    vector<string> tokens;
	
    stringstream ss(ip);
    string intermediate;
	
    while (getline(ss, intermediate, DELIM)) {
	    tokens.push_back(intermediate);
	}

	// Return false if tokens size is not equal to four
	if (tokens.size() != 4) {
	    return false;
	}

	// Validate each token
	for (unsigned int i = 0; i < tokens.size(); i++) {
	    
	// Verify that string is number or not and the numbers are in the valid range
	if (!IsNumber(tokens[i]) || atoi(tokens[i].c_str()) > 255 || atoi(tokens[i].c_str()) < 0) {
	        return false;
	    }
	}

	log(3, INFO, "Exiting", "IsValidIp");

	return true;
}


/******************************************************************************************************************************
FUNCTION NAME	: IsValidPortNum() 
       
DESCRIPTION		: This functions validates IP address. It returns true if valid and false if not valid
	
PARAMETER		: string ip
	
RETURN 			: bool
******************************************************************************************************************************/
bool IsValidPortNum(string portNum) {

	log(3, INFO, "Entering", "IsValidPortNum");
    
    // Verify that Port Number is number or not
    if (!IsNumber(portNum)) {

	log(3, DEBUG, "Entering", "IsValidPortNum"); 
        return false;
    }
    
	log(3, DEBUG, "Exiting", "IsValidPortNum");

    return true;
}

/********************************************************************************************************************************
FUNCTION NAME		: IsValidMobileNum() 
       
DESCRIPTION         : Validates the phone number based on the number of digits and the first number in the string.
			 		   The number is considered valid if it has got 10 digits and if the first number is 7 or 8 or 9. 
	
PARAMETER			: string mobNum
	
RETURN 				: bool
********************************************************************************************************************************/
bool IsValidMobileNum(string mobNum) {
    
	log(3, INFO, "Entering", "IsValidMobileNum");

    int len = 0;
    
    // Verify that Mobile Number contains digit or not
    if (!IsNumber(mobNum)) { 
        return false;
    }
    
    // Mobile number should be of 10 digits
    len = strlen(mobNum.c_str());
    if (len != 10) {
        return false;
    }
    
    // Mobile number should start from 7,8 or 9
    if (mobNum[0] < '7') { 
        return false;    
    }
    
	log(3, INFO, "Exiting", "IsValidPortNum");

    return true;
}


/******************************************************************************************************************************
DEFAULT CONSTRUCTOR	: Client 
       
DESCRIPTION     	: Sets the private data member sd to 0 and mobileNumber to NULL 

*******************************************************************************************************************************/
Client::Client() {

	//logfile.open("../dat/clientlog.txt", ios::out|ios::app);
	//	log(3, INFO, "Entering", "Default Constructor");

	sd = 0;
	mobileNumber = "";

	//	log(3, INFO, "Exiting", "Default Constructor");
}


/******************************************************************************************************************************
PARAMETERIZED CONSTRUCTOR	: Client 
       
DESCRIPTION			     	: Sets the private data members with the given values

*******************************************************************************************************************************/
Client::Client(string IpAddress, string PortNo, string mobNum) {

	logfile.open("../dat/clientlog.txt", ios::out|ios::app);

	log(3, INFO, "Entering", "Parameterized Constructor");

	// Populate the Server Address structure
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(PortNo.c_str())); 	// PORT NO
	serverAddress.sin_addr.s_addr = inet_addr(IpAddress.c_str());	// IP ADDRESS	
	mobileNumber = mobNum;	// MOBILE NUMBER

	log(3, INFO, "Exiting", "Parameterized Constructor");
}


/********************************************************************************************************************************
FUNCTION NAME	: OpenSocket() 
       
DESCRIPTION		: Creates an endpoint for communication and returns a non-zero file descriptor on successful 
		  		  socket creation that refers to the endpoint.
				  
PARAMETER		: void
	
RETURN 			: int
********************************************************************************************************************************/
int Client::OpenSocket() {

	log(3, INFO, "Entering", "OpenSocket");

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("SOCKET OPEN FAILURE");

		log(3, DEBUG, "OPENING SOCKET", "Open socket error");

		exit(0);
	}

	log(3, INFO, "Exiting", "OpenSocket");
	
	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: Connect() 
       
DESCRIPTION		: Connects the socket referred to by the file descriptor sd to the address specified 
     		      by the Server's address and the port
	
PARAMETER		: void
	
RETURN 			: int
********************************************************************************************************************************/
int Client::Connect() {

	log(3, INFO, "Entering", "Connect");
	
	int status = 0;
	
	// Connect
	status = connect(sd, (const struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if (status < 0) {
		perror("CONNECTION FAILURE");
		close(sd);
	
		log(3, DEBUG, "CONNECT SOCKET", "Connect error");

		exit(0);
	}
	
	cout << "Successfully Connected to the Server!" << endl;

	log(3, DEBUG, "Exiting", "Connect");
	
	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: IsMobNumRegistered() 
       
DESCRIPTION   	: Checks if the number entered is registered in the database. If present, show the menu !! 
		 		  Else, the number is registered in the database and the menu is shown.
	
PARAMETER		: string mobNum
	
RETURN 			: int
********************************************************************************************************************************/
int Client::IsMobNumRegistered(string mobNum) {

	log(3, INFO, "Entering", "IsMobNumRegistered");

	mobileNumber = mobNum;
	
	// '1' if Mobile Number is Registered or Not
	mobNum += ":1";
	
	SendData(const_cast<char*>(mobNum.c_str()));
	
	// Read the Data sent by Server
	ReadData();

	log(3, INFO, "Exiting", "IsMobNumRegistered");


	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: SendData() 
       
DESCRIPTION  	: Transmits message to another socket using send() function which contains file descriptor,
		 		   a buffer containing the message that is to be sent, the length of the buffer, a flag.  
	
PARAMETER		: char* data
	
RETURN 			: int
********************************************************************************************************************************/
int Client::SendData(char* data) {
	
	log(3, INFO, "Entering", "SendData");

	int count = 0, bufferLen = 0;
	char buffer[BUFFERSIZE];
	
	strcpy(buffer, data);
	bufferLen = strlen(buffer);
	
	// Send Data
	count = send(sd, buffer, bufferLen, 0);
	if (count < 0 ) {
		perror("DATA NOT SENT TO SERVER");
		close(sd);

		log(3, DEBUG, "SENDDATA", "Sending data error");

		exit(0);
	}
	
	// cout << "Data: '" <<  buffer << "' is sent to Server successfully!" << endl;

	log(3, INFO, "Exiting", "SendData");
	
	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: ReadData() 
       
DESCRIPTION		: Receives message from another socket using receive() function which contains file descriptor,
		 		  a buffer in which the message that is sent is stored, the length of the buffer, a flag.  
	
PARAMETER		: void
	
RETURN 			: int
********************************************************************************************************************************/
int Client::ReadData() {
	
	log(3, INFO, "Entering", "ReadData");

	socklen_t length;
	char buffer[BUFFERSIZE];
	length = sizeof(serverAddress);
	int count = 0;

	// Receive Data
	count = recv(sd, buffer, BLOCKSIZE, 0);
	if (count <= 0) {
		perror("DATA NOT RECEIVED FROM SERVER");
		close(sd);
		log(3, INFO, "READDATA", "Reading Data error");

		exit(0);
	}

	buffer[count] = '\0';
	
	// cout << "Data Received from Server: " << buffer <<  endl;
	cout << buffer << endl;
	
	log(3, DEBUG, "Exiting", "ReadData");

	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: MainMenu() 
       
DESCRIPTION   	: Displays all the services that a client can avail. Actions are taken based on the choice entered by the client  
	
PARAMETER   	: void
	
RETURN 			: int
********************************************************************************************************************************/
int Client::MainMenu() {

	log(3, INFO, "Entering", "MainMenu");
	
	// Choice
	int choice = 0;
	
	// Exit Message
	string msg = "Exit";
	
	// Performing Operation as per Client Choice
	do {
	    
		cout << endl;
	    cout << "************ MENU ************" << endl << endl;

	    cout << "1. ACTIVATE DND SERVICE" << endl;
	    cout << "2. DEACTIVATE DND SERVICE" << endl;
	    cout << "3. UPDATE DND SERVICE" << endl;
	    cout << "4. CALL ANOTHER CLIENT" << endl;
	    cout << "5. EXIT" << endl << endl;

	    cout << "******************************" << endl << endl;
	
	    cout << "Hi, which service would you like to avail? (1-5) : ";
	    cin >> choice;
	    
	    cout << endl;
	
	    switch(choice) {
	    
	        case 1	:	// cout << "Activate" << endl;
	                    ActivateService();
	                    break;
						
	        case 2	:	// cout << "Deactivate" << endl;
	                    DeactivateService();
	                    break;
	                    
	        case 3	:	// cout << "Update" << endl;
	                    UpdateService();
	                    break;
	                    
	        case 4	:	// cout << "Call" << endl;
						CallClient();
	                    break;
	                    
	        case 5	:	SendData(const_cast<char*>(msg.c_str()));
						break;
	                    
	        default :   cout << "Invalid Choice!" << endl; 
	                    break;
	    
	    }
	    
	} while (choice != 5);

	log(3, INFO, "Exiting", "MainMenu");
	
	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: IsServiceActivated() 
       
DESCRIPTION   	: Checks if the service is activated for this number. If activated, displays to the 
	       		  client that DND service is already avtivated. Else, send service request to the server.
	
PARAMETER		: void
	
RETURN 			: int
********************************************************************************************************************************/
int Client::IsServiceActivated() {

	log(3, INFO, "Enterting", "IsServiceActivated");
	
	string mobNum = "";
	char data[BUFFERSIZE];
    
	// '2' if Service is Activated or Not
	mobNum = mobileNumber + ":2";
	
	// Check at Server Side in Database
	// If Service is already Activated for the Mobile Number 
	SendData(const_cast<char*>(mobNum.c_str()));
	
	// Receive Data from Server
	int count = recv(sd, data, BLOCKSIZE, 0);
	if (count <= 0) {
		perror("DATA NOT RECEIVED FROM SERVER");
		close(sd);
		exit(0);
	}
	data[count] = '\0';

	// cout << "Yes/No : " << data << endl;
	
	if (strcmp(data, "yes") == 0) {
		return SUCCESS;
		
	} else if (strcmp(data, "no") == 0) {
		return FAILURE;
	}

	log(3, INFO, "Exiting", "IsServiceActivated");
	
	return FAILURE;
}


/********************************************************************************************************************************
FUNCTION NAME	: IsServiceActivated() 
       
DESCRIPTION		: Checks if the service is activated for this number. If activated, displays to the client 
		 		  that DND service is already activated. Else, send service request to the server.
	
PARAMETER		: void
	
RETURN 			: int
********************************************************************************************************************************/
int Client::ActivateService() {

	log(3, INFO, "Entering", "ActivateService");

	int serviceType = 0;
	string mobNum = "";
    int isActivated = 1;

	// Check if Service is Activated
	isActivated = IsServiceActivated();
	
	// Exit if Service is Already Activated
	if (isActivated == 0) {
		cout << "Can't Proceed as Service is Already Activated!" << endl;
		return FAILURE;

	} else {
		
		// Else Display Menu and Proceed Further
		cout << "***** TYPES OF SERVICES *****" << endl;
		cout << "1. GLOBAL DND" << endl;
		cout << "2. SELECTIVE DND" << endl;
		cout << "*****************************" << endl << endl;
			
		cout << "Which type of Service would you like to select?" << endl;
		cout << "Press 1 for Global and 2 for Selective : ";
		cin >> serviceType;

		while(serviceType < 1 || serviceType > 2) { 
			cout << "Invalid Choice! Please Enter Again (1/2) : ";
			cin >> serviceType;
		}
		
		cout << endl;
		
		// '3' if Global Service Activation is Requested
		mobNum = "";
		mobNum = mobileNumber + ":3";
		
		char service[BUFFERSIZE] = {0};
		string ch = "";
		string blockNum = "", num = "";

		switch (serviceType) {
						
			
			case 1	:	// Send Activation Request of 'Global Service' to Server		
						strcpy(service, mobNum.c_str());
						SendData(service);
						ReadData();
	                    break;

			case 2	:	// Send Activation Request of 'Selective Service' to Server
						do {
							
							// Input from the user the number he wishes to Block
							cout  << "Enter the Number you want to Block: ";
							cin >> num;
				
							while(!IsValidMobileNum(num)) {	
								cout << "Invalid Mobile Number!" << endl;
								cout << "Please Enter a Valid Number: ";
								cin >> num;
							}	
						
							blockNum.append(num);
						
							cout << "Do you want to Block another number?(y/n) : ";
							cin >> ch;
							
							while (ch != "y" && ch != "n") {
								cout << "Invalid Choice! Please Enter y or n : ";
								cin >> ch;
							}
							
							if (ch == "y") {
								blockNum.append(",");
							}

						} while (ch != "n");
						
						// '4' if Selective Service Activation is Requested
						mobNum = "";
						mobNum = mobileNumber + ":4" + ":" + blockNum;
						SendData(const_cast<char*>(mobNum.c_str()));
						ReadData();
						break;
			
			default :   cout << "Invalid Choice!" << endl;
						break;
		}
	} 

	log(3, INFO, "Entering", "ActivateService");

	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: DeactivateService() 
       
DESCRIPTION   : Checks if DND service is activated first. If activated ,deactivation of DND is initialized. 
                Else, displays that DND is not activated
	
PARAMETER	: void
	
RETURN 	: int
********************************************************************************************************************************/
int Client::DeactivateService() {

	log(3, INFO, "Entering", "DeactivateService");
	
	string data = "";
	string msg = "";
	int isActivated = 0;

	// Check if Service is Activated
	isActivated = IsServiceActivated();
	
	// Exit if Service is Already Deactivated
	if (isActivated == 1) {
		cout << "Can't Proceed as Service is Already Deactivated!" << endl;
		
		return FAILURE;

	} else {
		
		// '5' if Deactivation of Service is Requested
		msg = mobileNumber + ":5";
		SendData(const_cast<char*>(msg.c_str()));
		ReadData();
	}
	
	log(3, DEBUG, "Entering", "DeactivateService");
	
	return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: UpdateService() 
       
DESCRIPTION   : Checks if the service is activated for this number. If activated, displays the client that DND service that is
		  already avtivated and allows the client to change his type of service. Else, displays to activate the DND first.
	
PARAMETER	: void
	
RETURN 	: int
********************************************************************************************************************************/
int Client::UpdateService() {
	
	log(3, INFO, "Entering", "UpdateService");

	string msg = "";
	int isDeactivated = 1;
	
	// Deactivate Service
	isDeactivated = DeactivateService();

	if (isDeactivated == 0) {
	
		// Activate Service
		ActivateService();

		// '6' if Update of Service is Requested
		msg = mobileNumber + ":6";
		SendData(const_cast<char*>(msg.c_str()));
		ReadData();
	}
	
	log(3, INFO, "Exiting", "UpdateService");

    return SUCCESS;
}


/********************************************************************************************************************************
FUNCTION NAME	: CallClient() 
       
DESCRIPTION   : Call another client. Check if Receiver has subscribed for DND services and Establish or Block Call accordingly.
	
PARAMETER	: void
	
RETURN 	: int
********************************************************************************************************************************/
int Client::CallClient() {

	string msg = "";
	string callNum = "";
	
	// Input from the user the number he wishes to Call
	cout << "Enter which Number you want to Call: ";
	cin >> callNum;
	
	while(!IsValidMobileNum(callNum)) {	
		cout << "Invalid Mobile Number!" << endl;
		cout << "Please Enter a Valid Number: ";
		cin >> callNum;
	}	
	
	// '7' if Call to Client is Requested
	msg = mobileNumber + ":7" + ":" + callNum;
	SendData(const_cast<char*>(msg.c_str()));
	ReadData();
	
	return SUCCESS;
}


/********************************************************************************************************************************
DESTRUCTOR  	: ~Client() 
       
DESCRIPTION   : Closes the socket file descriptor 

********************************************************************************************************************************/
Client::~Client() {
	
	log(3, INFO, "Entering", "Destructor");

	int status = 0;
	
	// Closing the Socket Descriptor
	close(sd);
	if (status < 0) {
	    perror("SOCKET CLOSE FAILURE");

		log(3, DEBUG, "DESTRUCTOR", "Socket closing error");

	    exit(0);
	
	log(3, INFO, "Exiting", "Destructor");
	}
}
