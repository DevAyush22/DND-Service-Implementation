/**********************************************************************************
*	FILENAME     : servermain.c
*	DESCRIPTION  : Contains the main funtion of server that connects to the client and does actions
*				   based on the requests
*		
*  	AUTHOR		 : GR_McCARTHY_CPP_06
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
#include <signal.h>	//for signal handling

/**********************************************************************************
                       INCLUSION HEADER FILES
***********************************************************************************/
#include <server.h>
#include <logger.h>

/**********************************************************************************
                       MACROS
***********************************************************************************/

#define SUCCESS 0           // Returns 0 for Success
#define FAILURE 1           // Returns 1 for Failure

// Main Function
int main(int argc, char* argv[]) {

	log(3, INFO, "Entering", "Main");

	signal(SIGINT, signal_handler);	//calling the signal handle function to handle the interruption

	
	// Creating object of Server class
	Server s1;
	
	// Creation of a Socket
	s1.OpenSocket();

	// Bind Data
	s1.BindData();
	
	// Listen for Connections
	s1.Listen();
	
	log(3, INFO, "Exiting", "Main");
	return SUCCESS;
}