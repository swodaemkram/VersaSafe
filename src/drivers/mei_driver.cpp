/*
 * MEIValidator.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mark
 *
 *
 * The MEIValidator Comm setup is
 *            9600 E71
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <SerialStream.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <bits/stdc++.h>    // for strcpy
#include <array>            // for array copy
#include <iostream>
#include <vector>

#include "mei_driver.h"
#include "../hdr/global.h"
//#include "../trim.inc" //Looks as if I am unable to use Gary's trim function for some reason with my IDE no big deal
                         //more then one way to skin a cat
#include "../logs.inc"

using namespace std;
using namespace LibSerial;



/*
===============================================================================================================================
Start of MEI crc Function
===============================================================================================================================
*/
	class mei_do_crc{
	private:

	 mei_do_crc(char buff[], int buffer_len){

		 //printf("\n%02x%02x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]); //DEBUG CODE

		 //printf("%d\n",buffer_len); //DEBUG CODE


		 int i=1;
		 unsigned int sum = 0;

		 buffer_len = buffer_len - 2;


		        while ( i < buffer_len)
		        {
		           sum ^= buff[i];
		           //printf("%02x,%02x\n",sum,buff[i]); //DEBUG CODE
		           i++;
		        }
		        //printf("%d\n",buffer_len); //DEBUG CODE
	            //printf("the check sum = %02x\n",sum); //DEBUG CODE

		 //return(sum);

	};

/*
===============================================================================================================================
End of MEI crc Function
===============================================================================================================================
*/

// Call for driver version
//string mei_driver_version = ".08";
//string get_mei_driver_version(void)
//{
//    return mei_driver_version;
//}

/*
===============================================================================================================================
Setup Connection to MEI Validator (connection is 9600E71)
===============================================================================================================================
 */

class mei{

private:
string portname;
SerialStream my_serial;
bool detected=FALSE;
char buffer[200];

//Setup connection with MEI Validator
public:

int mei_connect(string pname)
		{


printf("Connect:open\n");
            my_serial.Open(portname);
            sprintf(buffer,"Connecting to MEI Validator ON %s",portname.c_str() );
            WriteSystemLog(buffer);
            if (my_serial.good() )
            {
printf("Serial good\n");
				sprintf(buffer,"Connected to MEI Validator ON %s",portname.c_str() );
				WriteSystemLog(buffer);
                my_serial.SetBaudRate(SerialStreamBuf::BAUD_9600);
                my_serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_7);
                my_serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
                my_serial.SetParity(SerialStreamBuf::PARITY_EVEN);
                my_serial.SetNumOfStopBits(1);
                return (1);		// success
            }

		sprintf(buffer,"Failed to Connect to MEI Validator ON %s",portname.c_str() );
		WriteSystemLog(buffer);
        return (0);

		}

// check the port string format
		bool CheckPort(string portname)
		{

					if ((portname.find("/dev/ttyS") != -1) || (portname.find("/dev/ttymxc") != -1) ||	(portname.find("/dev/ttyUSB") != -1) || (portname.find("/dev/ttyACM") != -1 ) )
					return TRUE;
					else
					return FALSE;

};



/*
===============================================================================================================================
 Connection to MEI Validator Completed
===============================================================================================================================
*/




















