/**********************************************************************************
*	FILENAME     : clientmain.c
*	DESCRIPTION  : Contains the main funtion of client that connects to the server and sends request as
*			 per his requirements.
*
*	AUTHOR		 : GR_McCARTHY_CPP_06
*	
*	DATE		 : 30 JANUARY,2020				
*	
*   COPYRIGHT  @ALTRAN.COM
*
***********************************************************************************/

/**********************************************************************************
                       STANDARD HEADER FILES
***********************************************************************************/
#include <iostream>	//Input output purposes
#include <cstdlib>	//for exit
#include <signal.h>	//for signal handling

/**********************************************************************************
                       INCLUSION HEADER FILES
***********************************************************************************/
#include <client.h>
#include <logger.h>

/*********************************************************************************
                       MACROS
**********************************************************************************/
#define SUCCESS 0           // Returns 0 for Success
#define FAILURE 1           // Returns 1 for Failure


// Main Function
int main(int argc, char* argv[]) {

	log(3, WARNING, "COMMANDLINE ARGUMENTS","Invalid Number of Arguments");

	signal(SIGINT, signal_handler); 

    // Validate Number of Inputs
    if (argc != 4) {
        cerr << "Invalid Arguments Passed!" << endl << "Usage: ./<Program_Name> <IP_Address> <Port_No> <Mobile_Number>" << endl;
		exit(0);
    }
	
    // Validate IP Address
    if (!IsValidIp(argv[1])) {

		log(3, DEBUG, "VALIDATING IP ADDRESS", "Invalid port number");

        cerr << "Invalid IP Address!" << endl;
		exit(1);
    }
	
    // Validate Port number
    if (!IsValidPortNum(argv[2])) {

		log(3, DEBUG, "VALIDATING PORT NUMBER", "Invalid port number");

        cerr << "Invalid Port Number!" << endl;
		exit(2);
    }
	
    // Validate Mobile Number
    if (!IsValidMobileNum(argv[3])) {

		log(3, DEBUG, "VALIDATING MOBILE NUMBER", "Invalid port number");

        cerr << "Invalid Mobile Number!" << endl;
		exit(3);
    }
	
    // Creating object of Client class
    // argv[1] = IP Address, argv[2] = Port Number, argv[3] = Mobile Number
    Client c1(argv[1], argv[2], argv[3]);	
	
    // Creation of a Socket
    c1.OpenSocket();
	
    // Connecting to the Server
    c1.Connect();
	
    // Send the mobile number to server, Check if the number is present in the file
    // If present, Display the menu
    // If not, Register the number and then Display the menu
    c1.IsMobNumRegistered(argv[3]);
	
    // Displaying the menu
    c1.MainMenu();

	log(3, INFO, "Exiting", "Main");

    return SUCCESS;	
}