#include <logger.h>

LogLevel glevel = DEBUG;
fstream logfile;
string getCurrentTime()
{
   string currTime;
   //Current date/time based on current time
   time_t now = time(0); 
   // Convert current time to string
   currTime.assign(ctime(&now));

   // Last charactor of currentTime is "\n", so remove it
   string currentTime = currTime.substr(0, currTime.size()-1);
   return currentTime;
}

void log(int arg,...)
{
	
    int level;		//stores the log level received in the function 
    string val;		//stores variable arguments received in the function
    string data;	//stores the log level in string to be stored in the log file
    bool display;	//checks whether to add string in logfile
    
    va_list vlist; 	//holds the information needed by va_start, va_arg, va_end
    va_start(vlist,arg);	//enables access to variadic function arguments
    
    //loop to access variable arguments   
    for(int i = 1; i <= arg; i++)
    {
        
        if (i == 1)
        {
            
                level = va_arg(vlist,int);	//accesses the next variadic function argument
                switch(level)		//checks for the log level
                {
                    case INFO : if (INFO <= glevel)
                                {
					
                                    data = "INFO";
                                    logfile << getCurrentTime() << " [" << data <<"]: "; 
                                    display = true;
                                }
                                else
                                {
                                    display = false;
                                    break;
                                }
                                break;

                    case WARNING : if (WARNING <= glevel)
                                   {
                                    	data = "WARNING";
                                    	logfile << getCurrentTime() << " [" << data <<"]: "; 
                                    	display = true;
                                   }
                                   else
                                   {
                                    	display = false;
                                    	break;
                                   }
                                   break;

                    case DEBUG : if (DEBUG <= glevel)
                                {
                                    data = "DEBUG";
                                    logfile << getCurrentTime() << " [" << data <<"]: "; 
                                    display = true;
                                }
                                else
                                {
                                   display = false;
                                   break;
                                }
                                break;
                }
                
           
        }
        else
        {
            if(display)
            {

                val = va_arg(vlist,char*);
                logfile << val <<" ";
            }
        }
    }
    if(display)
    {
        logfile <<endl;
    }
    va_end(vlist);	//ends traversal of the variadic function arguments
}
