/*
 ======================================================================================================================
 MEIValidator.cpp

 Created on: Apr 9, 2019
 Author: Mark Meadows

 A driver for the
 MEI Validator

 Comm setup is 9600 E 7 1
 No Flow Control
 =======================================================================================================================
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
#include "../trim.inc"   //Looks as if I am unable to use Gary's trim function for some reason with my IDE no big deal
                         //more then one way to skin a cat
#include "../logs.inc"

using namespace std;
using namespace LibSerial;

string mei_getresponse(); //get a response from the MEI Validator




/*
===============================================================================================================================
This was a function I used to make a map of were everything goes
===============================================================================================================================
*/
class mei {

public:
	string get_mei_driver_version(void){

		return "ver 00.00.80";
	}

/*
===============================================================================================================================
Start of MEI crc Function
===============================================================================================================================
*/

private:

unsigned do_crc(char buff[], int buffer_len){
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

	return(sum);
		}
/*
===============================================================================================================================
End of MEI crc Function
===============================================================================================================================
Connect to MEI Validator
===============================================================================================================================
*/
private:
char mei_buffer[200];
int mei_status;
SerialStream mei_my_serial;

public:
bool mei_detected;
string mei_portname;
bool detected=false;

		mei(string pname)
		{
			mei_detected=false;
			mei_portname=pname;
			mei_connect();
		}

		~mei(void)
		{
            if (mei_my_serial)
            mei_my_serial.Close();
			mei_detected=false;
		}

		// returns detected status
		bool isdetected(void)
		{
			return mei_detected;
		}

/*
===========================================================================================================================
Connect to MEI Validator Returns 1 on success 0 on fail
===========================================================================================================================
 */

int mei_connect(void)
		{

printf("Connect\n");
			// check the port before trying to use it
			if (!mei_checkport(mei_portname) )
			{
				sprintf(mei_buffer,"Unable to open %s for MEI validator",mei_portname.c_str() );
    	        WriteSystemLog(mei_buffer);
				mei_detected=true;
				return 0;
			}
	printf("Connect:open\n");
            mei_my_serial.Open(mei_portname);
            sprintf(mei_buffer,"Connecting to MEI validator ON %s",mei_portname.c_str() );
            WriteSystemLog(mei_buffer);
            if (mei_my_serial.good() )
            {
printf("Serial good\n");
				sprintf(mei_buffer,"Connected to MEI validator ON %s",mei_portname.c_str() );
				WriteSystemLog(mei_buffer);
                mei_my_serial.SetBaudRate(SerialStreamBuf::BAUD_9600);
                mei_my_serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_7);
                mei_my_serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
                mei_my_serial.SetParity(SerialStreamBuf::PARITY_EVEN);
                mei_my_serial.SetNumOfStopBits(1);
                mei_setup();
                return (1);		// success
            }

		sprintf(mei_buffer,"Failed to Connect to MEI validator ON %s",mei_portname.c_str() );
		WriteSystemLog(mei_buffer);
        return (0);

		}

		// check the port string format
		bool mei_checkport(string mei_portname)
		{

					if ((mei_portname.find("/dev/ttyS") != -1) || (mei_portname.find("/dev/ttymxc") != -1) ||	(mei_portname.find("/dev/ttyUSB") != -1) || (mei_portname.find("/dev/ttyACM") != -1 ) )
					return true;
					else
					return false;

		}
/*
===============================================================================================================================
 Connection to MEI Validator Completed
===============================================================================================================================
Private function check the connection status if connection haas been lost try to reconnect
===============================================================================================================================
*/

private:

bool mei_check_connection(void)
{

	if (mei_detected)
		return true;
	else
	{
		mssleep(2000);
		mei_connect();
	}
	return detected;
}

/*
===============================================================================================================================
MEI Validator Setup processed after the Comm port connection is made
===============================================================================================================================
 */

void mei_setup(void)
{
	string mei_rply ="";
	mei_rply = mei_getmodel();
	printf("MEI Validator model is a %s \n",mei_rply.c_str());


}
/*
================================================================================================================================
end of MEI Setup
================================================================================================================================
Get Response from MEI Validator
================================================================================================================================
 */

private:

string mei_getresponse(){

	string resp;
    printf("\nMEI GET RESPONSE CALLED\n");
    char next_char ;
    char input_buffer[150] ;
    bzero(input_buffer,150);
	int n=0;
	sleep(1);
	while (mei_my_serial.rdbuf()->in_avail() >0)
    {
        mei_my_serial.get(next_char);
        input_buffer[n++]=next_char;
        mssleep(1);
    }

    int len=strlen(input_buffer);
    printf("\nRcvd: %s --%d chars, last char: %02X\n",input_buffer,len,next_char);
    return string(input_buffer);

}
/*
===============================================================================================================================
End of Response Function
===============================================================================================================================
instantiating the mssleep class for the MEI class
===============================================================================================================================
*/

void mssleep(int micros)
{
    usleep(micros *1000);
}
/*
==============================================================================================================================
done with instantiating the mssleep class
==============================================================================================================================
Send data to MEI Validator
==============================================================================================================================
 */
private:

void mei_sendstring(string str)
{
		str = "";
		mei_my_serial << str;
}

/*
==============================================================================================================================
End of Send data to MEI
==============================================================================================================================
MEI Reset Command (Static does not change)
==============================================================================================================================
 */

public:

void mei_reset(void)
{
	int pktlen = 0;
	printf("MEI Reset Command Triggered\n");
	char pkt[16] = "\x02\x08\x60\x7f\x7f\x7f\x03\x17";
	pktlen = sizeof(pkt);
	printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial.write( pkt, pktlen ) ;

}
/*
=============================================================================================================================
End of MEI Reset Command
=============================================================================================================================
Get MEI Model Number (this is just a test and will become a GET INFO Command)
=============================================================================================================================
*/
public:

string mei_getmodel(void)
{
	int pktlen = 0;
	char pkt[16] = "\x02\x08\x60\x00\x00\x04\x03\x6c";
	pktlen = sizeof(pkt);
	printf("\nMEI GET MODEL CALLED...\n");
	string mei_rply1 = "";
	printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial.write( pkt, pktlen ) ;
	printf("\nMEI GET MODEL PACKET SENT");
	mei_rply1 = mei_getresponse();
	printf("\nmei_rply1 = %s",mei_rply1.c_str());
	return(mei_rply1);
}
/*
==============================================================================================================================
The End of the MEI Get Model Command
==============================================================================================================================
 */


};
/*
===============================================================================================================================
End of MEI Class
===============================================================================================================================
 */




















