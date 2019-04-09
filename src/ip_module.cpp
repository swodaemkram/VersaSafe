/*
	Module: ip_module.cpp
    Maestro Class Function Module
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 2-12-2012
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2012 FireKing Scurity
    Version: 1.0

	this module holds all the IP information, standard, legacy and used IP's

*/

using namespace std;


// PUBLIC FUNCTIONS
int ValidateIP(char *IPaddress);	// validate that IPaddress is a properly formed quad-triplet
void SetStandardIO(void);
void Set_IO_Types(void);


#include <string.h>
#include <stdio.h>
#include "hdr/config.h"




/*
	RESERVED IPS:	192.168.1.1, 192.168.1.20 (these are used for testing)


*/



#include <string.h>











/*
	validate that the passed IP address is a properly formed quad-triplet
	RETURNS:
		TRUE if IP is formatted correctly
		FALSE otherwise
*/


int ValidateIP(char *IPaddress)
{
	char *ptr=IPaddress;
	char ch = *ptr;
	int count=0;


	while( ch !=0)
	{
		if (!((ch >= '0' && ch <= '9') || ch == '.')) return FALSE;

		if (ch == '.')
			if (++count > 3) return FALSE;
		ch= *++ptr;
	}

	if (count == 3 && *--ptr != '.') return TRUE;

	return FALSE;
}








//EOM

