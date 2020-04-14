/**********************************************************************************
*       FILENAME    : server.cpp
*       DESCRIPTION : File contains definitions of the of server that receives message from clients
*			  		  as per their request
*
*	AUTHOR			: GR_McCARTHY_CPP_06
*	
*	DATE		 	: 30 JANUARY,2020				
*	
*   COPYRIGHT  @ALTRAN.COM
*
***********************************************************************************/

/**********************************************************************************
                       STANDARD HEADER FILES
***********************************************************************************/
#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

/**********************************************************************************
                       INCLUSION HEADER FILES
***********************************************************************************/

#include <server.h>
#include <logger.h>

/**********************************************************************************
                         		MACROS
***********************************************************************************/

#define BLOCKSIZE 512		// Block Size
#define BUFFERSIZE 1024		// Max Buffer Size
#define SERV_PORT 57322		// Port Number
#define LISTENQ 10			// Maximum Number of Client Connections
#define DELIM ':'			// Delimiter for Tokenizing Client Data

#define SUCCESS 0           // Returns 0 for Success
#define FAILURE 1           // Returns 1 for Failure


/********************************************************************************************************
* FUNCTION NAME : signal_handler
*
* DISCRIPTION : This function sends the signal on presssing Ctrl+C.
*
* ARGUMENTS : int
*
* RETURN TYPE : void
********************************************************************************************************/
void signal_handler(int sig) 
{ 
    cout << "Ctrl+C Sent: " << endl << "Server EXIT..."<<endl;  
    exit(EXIT_FAILURE);
}


/***********************************************************************************
							CONSTRUCTOR
************************************************************************************/
Server::Server() {

	logfile.open("../dat/serverlog.txt", ios::out|ios::app);

	log(3, INFO , "Entering", "Default Constructor");
	sd = 0;
	cd = 0;
	fd = 0;
	mobNumber = "";
	databuf[0] = '\0';
	clientIpAddress[0] = '\0';
	
	// Populating Own address structure to bind it to the Socket
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERV_PORT);	// PORT NO
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	// IP ADDRESS

	log(3, INFO , "Exiting", "Default Constructor");
}


/********************************************************************************************************************************
*	FUNCTION NAME	: OpenSocket() 
*       
*	DESCRIPTION     : Creates an endpoint for communication and returns a non-zero file descriptor on successful 
*  			          socket creation that refers to the endpoint.
*				  
*	PARAMETER	    : void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::OpenSocket() {

	log(3, INFO, "Entering", "OpenSocket");

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {

		perror("SOCKET OPEN FAILURE");

		log(3, DEBUG, "SOCKET CREATION", "Socket Error");

		exit(0);
	}
	
	log(3, INFO, "Exiting", "OpenSocket");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	: BindData() 
*       
*	DESCRIPTION     : Bins the socket to the address and port number specified in the address
*		        				  
*	PARAMETER		: void
*	
*	RETURN       	: int
********************************************************************************************************************************/
int Server::BindData() {

	log(3, INFO, "Entering", "BindData");

	int status = 0;
	
	// Now Binding
	status = bind(sd, (const struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if (status < 0) {
		perror("BIND FAILURE");
		close(sd);

		log(3, DEBUG, "BINDING SOCKET", "Bind Error");

		exit(0);
	}

	log(3, INFO, "Exiting", "BindData");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	: Listen() 
*       
*	DESCRIPTION     : Defines the maximum length to which the queue of pending connections for sockfd
*				 	  may grow and acceps the connections as per the queue
*		         			  
*	PARAMETER		: void
*	
*	RETURN 	        : int
********************************************************************************************************************************/
int Server::Listen() {

	log(3, INFO, "Entering", "Listen");

	int status = 0;
	
	cout << "Server Running... I am waiting for Connections..." << endl;
	
	// Listen
	status = listen(sd, LISTENQ);
	if (status < 0) {
		perror("LISTEN FAILURE");
		close(sd);

		log(3, DEBUG, "LISTENING SOCKET", "Listen Error");

		exit(0);
	}
	
	// Now waiting for Accepting Connection Request
	Accept();

	log(3, INFO, "Exiting", "Listen");
	
	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	: Accept()
*       
*	DESCRIPTION     : Extracts the first connection on the queue of pending connections, create a new socket
*					  with the same socket type protocol and address family as the specified socket
*				      
*		         			  
*	PARAMETER		: void
*	
*	RETURN 	        : int
********************************************************************************************************************************/
int Server::Accept() {

	log(3, INFO, "Entering", "Accept");

	memset(&clientAddress, 0, sizeof(clientAddress));
	
	socklen_t clientLength = sizeof(clientAddress);

	// Accept Connection
	cd = accept(sd, (struct sockaddr*) &clientAddress, &clientLength);
	if (cd < 0) {
		perror("ACCEPT FAILURE");
		close(sd);

		log(3, DEBUG, "ACCEPTING SOCKET", "Accept Error");

		exit(0);
	}
	
	// Printing the IP Address of the Client
	cout << endl << "Request from - Client IP Address: " << inet_ntop(AF_INET, &clientAddress.sin_addr, clientIpAddress, sizeof(clientIpAddress)) << endl;	
	
	// Read Data sent by Client
	ReadData();

	log(3, INFO, "Exiting", "Accept");
	
	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	: ReadData() 
*       
*	DESCRIPTION     : Receives message from another socket using receive() function which contains file descriptor,
*				  	a buffer in which the message that is sent is stored, the length of the buffer, a flag.  
*	
*	PARAMETER		: void
*	
*	RETURN 			: int
********************************************************************************************************************************/
int Server::ReadData() {

	log(3, INFO, "Entering", "ReadData");

	int count = 0;
	
	while(1) {

		// Receive Data from Client
		count = recv(cd, databuf, BUFFERSIZE, 0);
		if (count <= 0) {
			perror("DATA NOT RECEIVED FROM CLIENT");
			close(cd);
			close(sd);

			log(3, DEBUG, "READING DATA", "Read Error");

			exit(0);
		}
	
		databuf[count] = '\0';
	
		// cout << "Data Received from Client: " << databuf << endl;
		cout << databuf << endl;
	
		if (strcmp(databuf, "Exit") == 0) {
			break;
		}
		
		// Tokenize Data sent by Client
		TokenizeData(databuf);		
	}
	
	// Listen for New Connections
	Listen();

	log(3, INFO, "Exiting", "ReadData");
	
	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	: SendData() 
*       
*	DESCRIPTION     : Transmits message to another socket using send() function which contains file descriptor,
*					  a buffer containing the message that is to be sent, the length of the buffer, a flag.				   
*	
*	PARAMETER		: void
*	
*	RETURN 			: int
********************************************************************************************************************************/
int Server::SendData(char* data) {

	log(3, INFO, "Entering", "SendData");

	int count = 0, len = 0;
	len = strlen(data);

	count = send(cd, data, len, 0);
	if (count < 0) {
		perror("DATA NOT SENT TO CLIENT");
		close(cd);
		close(sd);
	
		log(3, DEBUG, "SENDING SOCKET", "Send Error");

		exit(0);
	}
	
	// cout << "Data: '" << data << "' is sent to Client successfully!" << endl << endl;

	log(3, INFO, "Exiting", "SendData");

	return SUCCESS;
}	


/********************************************************************************************************************************
*	FUNCTION NAME	: TokenizeData() 
*       
*	DESCRIPTION     : Token				   
*	
*	PARAMETER		: void
*	
*	RETURN 			: int
********************************************************************************************************************************/
int Server::TokenizeData(char* data) {	

	log(3, INFO, "Entering", "Tokenizedata");
	
	string str = data;
	
	// Split the string into tokens
	vector<string> tokens;
	
	stringstream ss(str);
	string intermediate;
	
	while(getline(ss, intermediate, DELIM)) {
		tokens.push_back(intermediate);
	}
	
	// Store Case (tokens[1])
	int choice = atoi(tokens[1].c_str());
	
	// Store Mobile Number
	mobNumber = tokens[0];
	
	switch(choice) {

		case 1	: 	// Check if Mobile Number is Registered
					IsNumberRegistered();
					break;
					
		case 2	:	// Check if Service is Activated for Mobile Number
					IsServiceActivated();
					break;
					
		case 3	:	// Process Activation Request for Global Service
					ProcessGlobalActivationRequest();
					break;

		case 4	:	// Process Activation Request for Selective Service 
					ProcessSelectiveActivationRequest(tokens[2]);
					break;
					
		case 5	:	// Process Deactivation Request for Mobile Number
					ProcessDeactivationRequest();
					break;
					
		case 6	:	// Process Update Service Request for Mobile Number
					ProcessUpdateRequest();
					break;
					
		case 7	:	// Process Call to another Client
					ProcessCallRequest(tokens[2]);
					break;					
	
		default	:	cout << "Invalid Choice!" << endl;
					break;				
	}

	log(3, INFO, "Exiting", "TokenizeData");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:IsNumberRegistered() 
*       
*	DESCRIPTION     :Check if Mobile Number is Registered or Not
*				  
*	PARAMETER       : void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::IsNumberRegistered() {

	log(3, INFO, "Entering", "IsNumberRegistered");

	int status = 0, isRegistered = 1;
	string msg;
	char buffer[BUFFERSIZE];
    string file = "../dat/RegisteredClients.txt";
    const char* clientFile = file.c_str();
    
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;

		log(3, DEBUG, "OPENING FILE", "Open File Error");

		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");

		log(3, DEBUG, "READ SOCKET", "Read Error");

		exit(0);
	}	

	stringstream ss(buffer);
	string mnumber;
	
	// Compare Mobile Number with each Number in File
	while (getline(ss, mnumber)) {
		
		if(mobNumber == mnumber) {
			isRegistered = 0;
		}
	}
	
	if (isRegistered == 0) {
		
		// If Number is Registered
		msg = "Mobile Number is already Registered!";
		SendData(const_cast<char*>(msg.c_str()));
	
	} else if (isRegistered == 1) {

		mobNumber.append("\n");
	
		// If Number is Not Registered
		string file = "../dat/RegisteredClients.txt";
		WriteFile(const_cast<char*>(mobNumber.c_str()), const_cast<char*>(file.c_str()));
		msg = "Mobile Number is successfully Registered into Database!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	// Close File Descriptor
	close(fd);

	log(3, INFO, "Exiting", "IsNumberRegistered");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:IsServiceActivated() 
      
*	DESCRIPTION     :Check if Mobile Number is Registered or Not
*				  
*	PARAMETER       : void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::IsServiceActivated() {

	log(3, INFO, "Entering", "IsServiceActivated");

	int status = 0, isActivated = 1;
	string msg;
	char buffer[BUFFERSIZE] = {0};
	string file = "../dat/ActivatedClients.txt";
    const char* clientFile = file.c_str();
	
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;

		log(3, DEBUG, "OPENING FILE", "Open File Error");

		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		isActivated = SearchMobNum(intermediate, mobNumber);
	}
	
	// Check if Service is Activated
	if (isActivated == 0) {
		msg = "yes";
		SendData(const_cast<char*>(msg.c_str()));
	} else if (isActivated == 1){
		msg = "no";
		SendData(const_cast<char*>(msg.c_str()));
	}

	log(3, INFO, "Exiting", "IsServiceActivated");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:IsServiceActivated() 
      
*	DESCRIPTION     :Search if Mobile Number exists or not
			  
*	PARAMETER       :string data, string mNum
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::SearchMobNum(string data, string mNum) {

	log(3, INFO, "Entering", "SearchMobNum");

	string msg;
	
	// Split the Data into Client Number
	vector<string> clientNum;
	
	stringstream ss(data);
	string intermediate;
	
	while(getline(ss, intermediate, ',')) {
		clientNum.push_back(intermediate);
	}
	
	// Return SUCCESS if Mobile Number exists
	if (clientNum[0] == mNum) {
		return SUCCESS;
	}
	
	log(3, INFO, "Exiting", "SearchMobNum");

	return FAILURE;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:ProcessGlobalActivationRequest()
      
*	DESCRIPTION     :To Process Activation Request for Global Service
			  
*	PARAMETER       :void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::ProcessGlobalActivationRequest() {

	log(3, INFO, "Entering", "ProcessGlobalActivationRequest");

	string msg;
	string file = "../dat/ActivatedClients.txt";
	string data = mobNumber + ",G" + "\n";

	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	msg = "Global Service successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));
	
	log(3, INFO, "Exiting", "ProcessGlobalActivationRequest");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:ProcessSelectiveActivationRequest()
      
*	DESCRIPTION     : To Process Activation Request for Selective Service
			  
*	PARAMETER       :string blockNum
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::ProcessSelectiveActivationRequest(string blockNum) {

	log(3, INFO, "Entering", "ProcessSelectiveActivationRequest");

	string msg;
	string file = "../dat/ActivatedClients.txt";
	string data = mobNumber + ",S," + blockNum + "\n";

	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	msg = "Selective Service successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));

	log(3, INFO, "Exiting", "ProcessSelectiveActivationRequest");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:ProcessDeactivationRequest()
      
*	DESCRIPTION     : To Process Deactivation Request for Mobile Number

			  
*	PARAMETER       :void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::ProcessDeactivationRequest() {

	log(3, INFO, "Entering", "ProcessDeactivationRequest");
	
	int status = 0;
	string msg = "";
	string data = "";
	char buffer[BUFFERSIZE] = {0};
	string file = "../dat/ActivatedClients.txt";
    const char* clientFile = file.c_str();

	// Clear Previous Records
	clientRecord.clear();
	
	// Opening the File in Read Mode
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");

		log(3, DEBUG, "READING FILE", "Read File Error");

		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		clientRecord.push_back(intermediate);
	}
	
	// Delete Record of Client if Mobile Number Exists
	for(unsigned int i = 0; i < clientRecord.size(); i++) {
		if (SearchMobNum(clientRecord[i], mobNumber) == 0) {

			// clientRecord[i].erase();
			clientRecord.erase(clientRecord.begin() + i);
		}
	}
	
	// Close File Descriptor
	close(fd);
	
	// Append Updated Data
	for(unsigned int i = 0; i < clientRecord.size(); i++) {
		data.append(clientRecord[i]);
		data.append("\n");
	}
	
	// Open File in Truncate Mode
	fd = open(clientFile, O_TRUNC);
	if (fd < 0) {
		perror("FILE OPENING FAILED!");

		log(3, DEBUG, "OPENING FILE", "Open File Error");

		exit(0);
	}
	
	// Close File Descriptor
	close(fd);
		
	// Write Updated Data to File
	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	// Send Message to Client
	msg = "DND Service is successfully Deactivated!";
	SendData(const_cast<char*>(msg.c_str()));

	log(3, INFO, "Exiting", "ProcessDeactivationRequest");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:ProcessUpdateRequest()
      
*	DESCRIPTION     : Process Update Service Request for Mobile Number
			  
*	PARAMETER       :void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::ProcessUpdateRequest() {

	log(3, INFO, "Entering", "ProcessUpdateRequest");
	
	string msg = "";
	
	// Send Message to Client
	msg = "DND Service is successfully Updated!";
	SendData(const_cast<char*>(msg.c_str()));
	
	log(3, INFO, "Exiting", "ProcessUpdateRequest");

	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:ProcessCallRequest()
      
*	DESCRIPTION     : Process Call to another Client
			  
*	PARAMETER       :void
*	
*	RETURN          : int
********************************************************************************************************************************/
int Server::ProcessCallRequest(string callNum) {
	
	int status = 0, isNumPresent = 1, isActivated = 1;
	string msg = "", serviceType = "";
	char buffer[BUFFERSIZE] = {0};
	string file = "../dat/ActivatedClients.txt";
    const char* clientFile = file.c_str();
	
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		isActivated = SearchMobNum(intermediate, callNum);
		
		// Check if DND Service is Activated for Receiver
		if (isActivated == 0) {
		
			// Split the Record
			vector<string> record;
			record.clear();
		
			stringstream st(intermediate);
			string data;
			
			while (getline(st, data, ',')) {
				record.push_back(data);
			}
		
			// Store DND Service Type
			serviceType = record[1];
		
			// Block all Calls if Service Type is Global
			if (serviceType == "G") {
			
				msg = "Call Blocked as '" + callNum + "' Receiver has subscribed for Global DND Services!";
				SendData(const_cast<char*>(msg.c_str()));
				break;
				
			// Block selected Calls if Service Type is Selective
			} else if (serviceType == "S") {
				
				for (unsigned int i = 2; i < record.size(); i++) {
					if (record[i] == mobNumber) {
						isNumPresent = 0;
						msg = "Call Blocked as '" + callNum + "' Receiver has subscribed for Selective DND Services!";
						SendData(const_cast<char*>(msg.c_str()));
						break;
					}
				}
				break;
			}
		}
	}
	
	// Establish Call if DND Service isn't Activated for Receiver
	if (isActivated == 1) {
		
		// Send Message to Caller Client
		msg = "Call Successfully Established with '" + callNum + "' Mobile Number!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	// Establish Call if Receiver hasn't listed Caller Number in case of Selective
	if (serviceType == "S" && isNumPresent == 1) {
		
		// Send Message to Caller Client
		msg = "Call Successfully Established with '" + callNum + "' Mobile Number!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	return SUCCESS;
}


/********************************************************************************************************************************
*	FUNCTION NAME	:WriteFile
      
*	DESCRIPTION     :Write Data to File
			  
*	PARAMETER       :char* data, char* fname
*	
*	RETURN          : int
********************************************************************************************************************************/

int Server::WriteFile(char* data, char* fname) {

	log(3, INFO, "Entering", "WriteFile");

    
    int status = 0;
    
	// Opening the File
	fd = open(fname, O_WRONLY|O_APPEND, S_IRWXU);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		
		log(3, DEBUG, "WRITING FILE", "Write File Error");

		exit(0);
	}

    // Write to File
    status = write(fd, data, strlen(data));
	if (status < 0) {
	 	perror("FILE WRITE FAILURE!");
        exit(0);
	}
	
	// Close File Descriptor
	close(fd);

	log(3, INFO, "Exiting", "WriteFile");


    return SUCCESS;
}

	
/********************************************************************************************************************************
*	FUNCTION NAME	:~Server()
      
*	DESCRIPTION     :Destructor
			  
*	PARAMETER       :void
*	
*	RETURN          :void
********************************************************************************************************************************/
Server::~Server() {

	log(3, INFO, "Entering", "Destructor");

	int sstatus = 0, cstatus = 0;

	// Closing the Connection Descriptor
	cstatus = close(cd);
	if (cstatus < 0) {
		perror("CONNECTION CLOSE FAILURE");

		log(3, DEBUG, "CONNECTION CLOSE", "Close Error");

		exit(0);
	}

	// Closing the Socket Descriptor
	sstatus = close(sd);
	if (sstatus < 0) {
		perror("SOCKET CLOSE FAILURE");
		exit(0);
	}

	log(3, INFO, "Exiting", "Destructor");
}
