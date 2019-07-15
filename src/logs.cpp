/*
	Module: logs.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 9-27-2012
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2018 FireKing Security
    Version: 1.0


	this module handles message logging and display

*/



using namespace std;


#include "hdr/fire.h"

// C++
#include <string>
#include <stdint.h>
#include <stdio.h>

char syslog[]="/var/log/fireking.log";
char partlog[] = "../logs/parthistory.log";


// trim.cpp
#include "trim.inc"

// common.cpp
extern int WriteFile(char * filename, char * message);
extern char * TimeStamp(void);
extern uintmax_t EpochTime(void);

extern OperationMode OpMode;


// locale.cpp
extern string getMessage(int msgID, int add_msgnum=TRUE);




// PUBLIC FUNCTIONS
bool WriteSystemLog(char * newmsg);
bool WriteSystemLog(string newmsgstring);
bool WritePartLog(char * newmsg);




/*
	write a system log message
	format is: datestamp, msg, opmode

	RETURNS: TRUE on success, else FALSE		

*/


bool WriteSystemLog(char * newmsg)
{
	string msg;
	char sysmsg[500];

	chomp(newmsg);
/*
	switch(OpMode)
	{
	case Manual_mode:
		msg = getMessage(620,FALSE);
		break;
	case Load_mode:
		msg = getMessage(621,FALSE);
		break;
	case Unload_mode:
		msg = getMessage(622,FALSE);
		break;
	case CMC_mode:
		msg = getMessage(623,FALSE);
		break;
	case Prod_mode:
		msg = getMessage(624,FALSE);
		break;
	default:
		msg = "unknown";
		break;
	}
*/
	char *ptr = TimeStamp();
	chomp(ptr);
	


//    TimeStamp() creates a nix timestamp as follows
//    Sat Jan 27 10:32:12 2007
	sprintf(sysmsg,"%s,%s,%s", ptr, newmsg, msg.c_str() );

	// returns OK or ERROR
	if ( WriteFile(syslog,sysmsg) == OK)
		return TRUE;

	return FALSE;
}

/*
==========================================================================================================================
Over Loaded By Mark Meadows to accept a string value
==========================================================================================================================
 */

bool WriteSystemLog(string newmsgstring)
{
	char sysmsg[500];
	char *ptr = TimeStamp();
	sprintf(sysmsg,"%s,%s",  ptr, newmsgstring.c_str());
	if ( WriteFile(syslog,sysmsg) == OK) return TRUE;
	return FALSE;

}
/*
==========================================================================================================================
End of Over Loaded Function By Mark Meadows to accept a string value
==========================================================================================================================
 */




/*
	every time a new recipe is started, log it
	format is: datestamp, originator, opmode, recipe name

	originator can be "FlowForm"

	conditions:

		enter PARTLOAD mode


	RETURNS: TRUE on success, else FALSE		
*/

bool WritePartLog(char * newmsg)
{
	string msg;
	char sysmsg[500];
	chomp(newmsg);

	switch(OpMode)
	{
	case Manual_mode:
		msg = getMessage(620,FALSE);
		break;
	case Load_mode:
		msg = getMessage(621,FALSE);
		break;
	case Unload_mode:
		msg = getMessage(622,FALSE);
		break;
	case CMC_mode:
		msg = getMessage(623,FALSE);
		break;
	case Prod_mode:
		msg = getMessage(624,FALSE);
		break;
	default:
		msg = "unknown";
		break;
	}


//    TimeStamp() creates a nix timestamp as follows
//    Sat Jan 27 10:32:12 2007

	char *ptr = TimeStamp();
	chomp(ptr);

//	sprintf(sysmsg,"%s,%s,%s,%s",ptr, newmsg, msg.c_str(), active_recipe_name );


	// returns OK or ERROR
	if ( WriteFile(partlog,sysmsg) == OK)
		return TRUE;

	return FALSE;

}















