/*
	PID handlers
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 12-5-2017
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2017 FireKing Security Group


	This module handles creating, removing or checking existence of a PID file

*/


#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "hdr/fire.h"


using namespace std;

const char * PIDfile[]= {
					"/var/run/versasafe.pid",
			};

/*
	defined in "fire.h"

enum
{
	VERSASAFE_PID=0,

};
*/

bool CheckRunning(int pid_ID);		// check to see if versasafe is running, if so, return TRUE
bool createPIDfile(int pid_ID);		// create the PID file
bool removePIDfile(int pid_ID);		// remove the PID file

/*
	creates our PID file
	RETURNS: 	TRUE on success
				FALSE otherwise

*/
bool createPIDfile(int pid_ID)
{

    FILE *fp;
    int x;

    int my_pid=getpid();

	printf("Creating PID file for %d\n",my_pid);

	// this should never happen because we have already called CheckRunning() previously
    fp=fopen(PIDfile[pid_ID],"r");
    if (fp != NULL)
    {
	// file exists, close it
        fclose(fp);
		unlink(PIDfile[pid_ID]);
		printf("Process is already running, deleting PID.\n");
    }

    // create the PID file
printf("%s\n",PIDfile[pid_ID]);
    fp = fopen(PIDfile[pid_ID],"w");
    if (fp != NULL)
    {
        x= fprintf(fp,"%u\n",my_pid);
        if (x< 0 )
        {
            printf("Error writing to PID file\n");
			fclose(fp);
			return FALSE;
        }

        if (fclose(fp) ==0)
            printf("PIDfile created successfully\n");
        else
		{
            printf("Error closing PID file\n");
	        return FALSE;
		}
    }
    else
    {
		printf("Error, unable to create PID file\n");
		return FALSE;
    }

    return TRUE;
}



/*
	returns TRUE if process is already running
*/

bool CheckRunning(int pid_ID)
{
    FILE * fp;
       // see if the process is already running by checking for the PID file
        // if there, then exit gracefully
        fp=fopen(PIDfile[pid_ID],"r");
        if (fp != NULL)
        {
            fclose(fp);
            printf("\nProcess is already running.\n");
			return TRUE;
        }


	return FALSE;
}




/*
	remove the PID file
*/

bool removePIDfile(int pid_ID)
{
	unlink(PIDfile[pid_ID]);
	printf("PID file removed.\n");
}













//EOM

