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

#include <time.h>
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
#include <SerialPort.h>
#include "mei_driver.h"
#include "../hdr/global.h"
#include "../trim.inc"   //Looks as if I am unable to use Gary's trim function for some reason with my IDE no big deal
                         //more then one way to skin a cat
#include "../logs.inc"

using namespace std;
using namespace LibSerial;

string mei_getresponse(); //get a response from the MEI Validator
unsigned int mei_do_crc(char buff[], int buffer_len); //Preform crc on packet
string mei_poll(void); //Poll MEI Device
void mei_reset(void);
string get_mei_driver_version(void);
string mei_verify_bill(void);


SerialStream mei_my_serial; //THIS MUST BE ABOVE THE CLASS TO KEEP IT GLOBAL!

//Version of the Driver
	string get_mei_driver_version(void){
		return "ver 00.00.91";
	}

/*
===============================================================================================================================
Start of the MEI Class
===============================================================================================================================
*/
class mei {
/*
===============================================================================================================================
Start of MEI crc Function
===============================================================================================================================
*/

private:

unsigned int mei_do_crc(char buff[], int buffer_len){
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
//SerialStream mei_my_serial;

public:
bool mei_detected;
string mei_portname;
bool detected=false;

public:
		mei(string pname)
		{
			mei_detected=false;
			mei_portname=pname;
			mei_connect();
		}

		~mei(void)
		{
            printf("\nCloseing Comm port!\n");
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
public:

	int mei_connect(void)
		{

printf("Connecting.....\n");
			// check the port before trying to use it
			if (!mei_checkport(mei_portname) )
			{
				sprintf(mei_buffer,"Unable to open %s for MEI validator",mei_portname.c_str() );
    	        WriteSystemLog(mei_buffer);
    	        mei_detected = false;
    	        return 0;
			}
	printf("Connect:open\n");
            mei_my_serial.Open(mei_portname);
            sprintf(mei_buffer,"Connecting to MEI validator ON %s",mei_portname.c_str() );
            WriteSystemLog(mei_buffer);
            if (mei_my_serial.good() )
            {
printf("Serial good\n");
				printf("MEI Connected to %s Port\n",mei_portname.c_str());
				sprintf(mei_buffer,"Connected to MEI validator ON %s",mei_portname.c_str() );
				WriteSystemLog(mei_buffer);
                mei_my_serial.SetBaudRate(SerialStreamBuf::BAUD_9600);
                mei_my_serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_7);
                mei_my_serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
                mei_my_serial.SetParity(SerialStreamBuf::PARITY_EVEN);
                mei_my_serial.SetNumOfStopBits(1);
                mei_setup();
                printf("MEI Validator Connected....");
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
private:

void mei_setup(void)
{
	string mei_rply ="";
	mei_rply = mei_getmodel();
	//mei_reset();
	//mei_rply = mei_verify_bill();
	printf("MEI Validator model is a %s \n",mei_rply.c_str());
	mei_detected = true;
	return;
}
/*
================================================================================================================================
end of MEI Setup
================================================================================================================================
Get Response from MEI Validator This is the raw Hex data and needs to be processed in the returning function
================================================================================================================================
 */

private:

string mei_getresponse(){

	int buff_size = 250 ;
	char input_buffer[buff_size] ;
	bzero(input_buffer,buff_size);
	int char_pos = 0;
	char next_char = {0};

	//printf("\nMEI GET RESPONSE CALLED\n");

	mssleep(300); //This delay is very important it wont work without it. So I found out after several hours!
	while (mei_my_serial.rdbuf()->in_avail() )
	{

				mei_my_serial.get(next_char);
		        if (next_char == '\03')
				{
					input_buffer[char_pos++]='\03';
					mei_my_serial.get(next_char);
					input_buffer[char_pos++]=next_char;
					break;
				}
			   input_buffer[char_pos] = next_char;
		       char_pos++;

		}
				//printf("char_pos = %d\n",char_pos); [DEBUG]
				char sent_crc = {0};
                sent_crc = input_buffer[char_pos - 1];
				//printf("sent_crc = %02x\n",sent_crc); [DEBUG]
				int thecrc = 0;

				thecrc = 0;
				thecrc =  mei_do_crc(input_buffer,char_pos);
				//printf("the crc = %02x\n",thecrc); [DEBUG]

//TODO  need to do something if the crc doesn't match

			//printf("Hex Response from MEI = "); [DEBUG]
			int w = 0;
			while(w <= (char_pos -1) ){
				printf("%02x",input_buffer[w]);

				printf("|");
				w++;
			}
		printf("\n");

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

	if(!mei_my_serial) mei_connect();
	int pktlen = 0;
	printf("MEI Reset Command Triggered\n");
	char pkt[] = {0x02,0x08,0x60,0x7f,0x7f,0x7f,0x03,0x17};
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial << pkt;//<----DO NOT DO THIS ANY MORE USE mei_my_serial.write instead

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
	printf("MEI GetModel called\n");
	char pkt[] = {0x02,0x08,0x60,0x00,0x00,0x04,0x03,0x6c};
	string mei_rply1 = "";
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial.write(pkt, sizeof(pkt)) ;
	mei_rply1 = mei_getresponse();
	char pktAk[16] = {0x02,0x08,0x01,0x00,0x00,0x04,0x03,0x0d}; //ACK Packet to send
	//mei_my_serial << pktAk;
	mei_my_serial.write(pktAk, sizeof(pktAk)) ;
	//TODO need to remove all the non-printable characters before returning this and the rest of these responses
	return(mei_rply1);
}
/*
==============================================================================================================================
The End of the MEI Get Model Command
==============================================================================================================================
MEI Verify Bill Command
==============================================================================================================================
 */

public:

string mei_verify_bill()
{

	printf("MEI Verify Bill called\n");
	string mei_poll_response = {0};
	unsigned int thecrc;
	string mei_rply1 = {0};
	char pkt[16] = "\x02\x08\x10\x2c\x7f\x10\x00";  // <-- Packet without ETX or CRC //working \x02\x08\x10\x2c\x7f\x00\x03
	thecrc =  mei_do_crc(pkt,sizeof(pkt));          //Do CRC
	pkt[6] = '\x03';                                //Stuff ETX
	pkt[7] = thecrc;                                //Stuff CRC
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial.write( pkt, sizeof(pkt)) ;         //Send Command
	mei_rply1 = mei_getresponse();                   //Get Reply
	char pktAk[16] ="\x02\x08\x01\x2c\x7f\x10\x00"; //ACK Packet to send
	thecrc = 0;                                      // Zero Out CRC
	thecrc =  mei_do_crc(pktAk,sizeof(pktAk));         //Do CRC on new packet
	pktAk[6] = '\x03';                                 //Stuff ETX
	pktAk[7] = thecrc;                                 //Stuff CRC
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pktAk[0],pktAk[1],pktAk[2],pktAk[3],pktAk[4],pktAk[5],pktAk[6],pktAk[7]);
	mei_my_serial.write(pktAk, sizeof(pktAk)) ;
	mei_rply1 = "";
	mei_rply1 = mei_getresponse();
    mei_poll_response = mei_poll();                    // Poll MEI for response to command



}
/*
==============================================================================================================================
The End of the MEI Verify Bill Command
==============================================================================================================================
 MEI Poll Command
==============================================================================================================================
 */
private:

string mei_poll(void){

//TODO MEI POLL UNTIL BILL IS VERIFIED of course I need some way to escape this while loop this is for DEBUG only
	unsigned int thecrc;
	string mei_rply1 = {0};

	while(1){
	char pollpkt[16] = "\x02\x08\x10\x1f\x14\x00\x00" ;   //Poll packet
	thecrc = 0;
	thecrc =  mei_do_crc(pollpkt,sizeof(pollpkt));          //Do CRC
	pollpkt[6] = '\x03';                                //Stuff ETX
	pollpkt[7] = thecrc;                                //Stuff CRC
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pollpkt[0],pollpkt[1],pollpkt[2],pollpkt[3],pollpkt[4],pollpkt[5],pollpkt[6],pollpkt[7]);
	mei_my_serial.write( pollpkt, sizeof(pollpkt)) ;         //Send Command
	mei_rply1 = mei_getresponse();                   //Get Reply
	char pollpktAk[16] ="\x02\x08\x11\x1f\x14\x00\x00"; //ACK Packet to send
	thecrc = 0;                                      // Zero Out CRC
	thecrc =  mei_do_crc(pollpktAk,sizeof(pollpktAk));         //Do CRC on new packet
	pollpktAk[6] = '\x03';                                 //Stuff ETX
	pollpktAk[7] = thecrc;                                 //Stuff CRC
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pktAk[0],pktAk[1],pktAk[2],pktAk[3],pktAk[4],pktAk[5],pktAk[6],pktAk[7]);
	mei_my_serial.write(pollpktAk, sizeof(pollpktAk)) ;
	mei_rply1 = "";
	mei_rply1 = mei_getresponse();

	if (mei_rply1[5] =='\x08'){
		printf("\nI Verified a $1.00 Bill!\n\n");
		exit(0);

	}



	}
}
/*
==============================================================================================================================
The End of the MEI Poll Command
==============================================================================================================================
 */
};
/*
===============================================================================================================================
End of MEI Class
===============================================================================================================================
 */




















