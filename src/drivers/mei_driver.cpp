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
//string get_mei_driver_version(void);
string mei_verify_bill(void);
void mei_stack_documents(void);

//SerialStream mei_my_serial; //THIS MUST BE ABOVE THE CLASS TO KEEP IT GLOBAL!

//Version of the Driver//KEEP THIS GLOBAL ALSO
//	string get_mei_driver_version(void){
//		return "ver 00.00.99";
//	}

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
SerialStream mei_my_serial;
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
	//mei_rply = mei_getmodel();
	mei_reset();
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
	char status;
	char newstatus;
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
//------------------------------------------------------------------------------------------------
//    Check CRC of RXed packed
//------------------------------------------------------------------------------------------------

	//printf("char_pos = %d\n",char_pos); [DEBUG]
				char sent_crc = {0};
                sent_crc = input_buffer[char_pos - 1];
				//printf("sent_crc = %02x\n",sent_crc); [DEBUG]
				int thecrc = 0;
				thecrc = 0;
				thecrc =  mei_do_crc(input_buffer,char_pos);
				//printf("the crc = %02x",thecrc); //[DEBUG]
				int thesize = strlen(input_buffer) - 1;
				//printf("The rxed crc = %02x\n",input_buffer[thesize]);
				if (thecrc != input_buffer[thesize]){
					return("");
				}
//-------------------------------------------------------------------------------------------------
//End of CRC check of Packet
//-------------------------------------------------------------------------------------------------
// Print Rxed Packet in Hex so we can look at it
//-------------------------------------------------------------------------------------------------
//           printf("Hex Response from MEI = "); [DEBUG]
			int w = 0;
			while(w <= (char_pos -1) ){
				printf("%02x",input_buffer[w]);
				printf("|");
				w++;
			}
//-------------------------------------------------------------------------------------------------
//Translate Data to English
//-------------------------------------------------------------------------------------------------
// Proccess Byto 0
//-------------------------------------------------------------------------------------------------
			switch (input_buffer[3])
			{

			case '\x00':
				printf(" MEI Validator is Idling");
				break;

			case '\x01' :
				printf("\033[1;34m MEI Validator Accepting\033[0m");
				break;

			case '\x02' :
				printf("\033[1;32m MEI Validator Drawing a Note/Barcode in\033[0m");
				break;

			case '\x04' :
				printf("\033[0;31m MEI Validator is Escrowed\033[0m");
				break;

			case '\x08' :
				printf(" MEI Validator is Stacking");
				break;

			case '\x16'	:
				printf(" MEI Validator has Stacked");
				break;

			case '\x20' :
				printf("\033[1;33m MEI Validator is Processing\033[0m");
				break;

			case  '\x41':
				printf("\033[1;32m MEI Validator has Returned\033[0m");
				break;

			}
//----------------------------------------------------------------------------------------------
//End of Processing Byte 0
//----------------------------------------------------------------------------------------------
// Process Byte 1
//----------------------------------------------------------------------------------------------
			switch (input_buffer[4])
						{

						case '\x01' :
							printf(" | MEI Validator Cheated !");
							break;

						case '\x02' :
							printf(" | MEI Validator Rejected !");
							break;

						case '\x04' :
							printf(" | MEI Validator is Jamed");
							break;

						case '\x08' :
							printf(" | MEI Validator is Full");
							break;

						case '\x10'	:
							printf(" | MEI Validator Cassette has been removed");
							break;

						}
//---------------------------------------------------------------------------------------------
// End of Processing Byte 1
//---------------------------------------------------------------------------------------------
//Processing Byte 3
//---------------------------------------------------------------------------------------------
			switch (input_buffer[5])
									{

									case '\x01' :
										printf(" | MEI Validator Power up !");
										break;

									case '\x04' :
										printf(" | MEI Validator Failure !");
										break;
									}
//----------------------------------------------------------------------------------------------
//End of Processing Byte 3
//----------------------------------------------------------------------------------------------
//End of English Translation
//----------------------------------------------------------------------------------------------
			printf("\n");

return string(input_buffer);
}

/*
===============================================================================================================================
End of Response Function
===============================================================================================================================
the mssleep Function for the MEI class
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
	string cleantext = "";
	printf("MEI GetModel called\n");
	char pkt[] = {0x02,0x08,0x60,0x00,0x00,0x04,0x03,0x6c};
	string mei_rply1 = "";
	//printf("This is the cmd packet I'm sending --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial.write(pkt, sizeof(pkt)) ;
	mei_rply1 = mei_getresponse();
	//cleantext = mei_clean_string(mei_rply1, sizeof(mei_rply1));

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

string mei_verify_bill(void)
{

	printf("MEI Verify Bill called\n");
	mei_reset();                                    //Reset the MEI
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
   	return(mei_poll_response);

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
//We now time out after 30 seconds if no valid bill is detected
	unsigned int thecrc;
	string mei_rply1 = {0};
	time_t starttime = time(0);
	string mei_old = {0};

		while(1){

         if (mei_rply1[3] != mei_old[3]){                              //reset time out if MEI is doing something
        	 time_t starttime = time(0);
        	 mei_old[3] = mei_rply1[3];
         }

		time_t currenttime = time(0);
		int difftime = currenttime - starttime;
		//cout<<difftime<<endl;
		if (difftime >= 30) {
			printf("\nTIME OUT!\n");
			return("time out");
		}

		char pollpkt[8] = "\x02\x08\x10\x7f\x5c\x00\x00" ; //NEW POLL PACKET
		thecrc = 0;
		thecrc =  mei_do_crc(pollpkt,sizeof(pollpkt));          //Do CRC
		pollpkt[6] = '\x03';                                //Stuff ETX
		pollpkt[7] = thecrc;                                //Stuff CRC
		//printf("This is the cmd packet I'm sending 1st --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pollpkt[0],pollpkt[1],pollpkt[2],pollpkt[3],pollpkt[4],pollpkt[5],pollpkt[6],pollpkt[7]);
		mei_my_serial.write( pollpkt, sizeof(pollpkt)) ;         //Send Command
		mei_rply1 = mei_getresponse();                   //Get Reply
		mei_ack(pollpkt, sizeof(pollpkt));
		mei_rply1 = "";
		mei_rply1 = mei_getresponse();

//---------------------------------------------------------------------------------------------------------------
// We Verified a $1 dollar bill
//---------------------------------------------------------------------------------------------------------------
		if (mei_rply1[5] =='\x08'){
			printf("\nI Verified a $1.00 Bill!\n\n");
			return("Verified a $1 Bill");
		}
//---------------------------------------------------------------------------------------------------------------
// We Verified a $20 dollar bill
//---------------------------------------------------------------------------------------------------------------
		if (mei_rply1[5] =='\x28'){
			printf("\nI Verified a $20.00 Bill!\n\n");
			return("Verified a $20 Bill");
		}
//---------------------------------------------------------------------------------------------------------------

	}
}
/*
==============================================================================================================================
The End of the MEI Poll Command
==============================================================================================================================
 MEI ACK command will send ack for any packet RXed
==============================================================================================================================
 */
private:

void mei_ack(char packettoAK[], int packettoAKLen){

			int thecrc = 0;
			packettoAK[2] = '\x11';
			packettoAK[packettoAKLen - 1]= '\x00';//remove CRC
			packettoAK[packettoAKLen - 2]= '\x00';//remove ETX
			//printf("Sending to CRC --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",packettoAK[0],packettoAK[1],packettoAK[2],packettoAK[3],packettoAK[4],packettoAK[5],packettoAK[6],packettoAK[7]);
			thecrc =  mei_do_crc(packettoAK,packettoAKLen);//Do CRC
			packettoAK[packettoAKLen - 1] = thecrc; //stuff the crc back in
			packettoAK[packettoAKLen - 2] = '\x03'; //stuff the ETX back in
			mei_my_serial.write( packettoAK, packettoAKLen) ;         //Send Command
			//printf("This is the ACK packet I'm sending--> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",packettoAK[0],packettoAK[1],packettoAK[2],packettoAK[3],packettoAK[4],packettoAK[5],packettoAK[6],packettoAK[7]);
			return;
}
/*
==================================================================================================================================
End of MEI ACK
==================================================================================================================================
Return Document to customer
==================================================================================================================================
 */
public:

void mei_return_document(void){

	int thecrc = 0;
	char pkt[] = {0x02,0x08,0x10,0x00,0x5c,0x00,0x00,0x00};//Packet skel
	thecrc =  mei_do_crc(pkt,sizeof(pkt));//Do CRC
	pkt[6] = '\x03'; //Stuff EXT
	pkt[7] = thecrc; //stuff CRC
	//printf("Command to Return Doc --> %02x%02x%02x%02x%02x%02x%02x%02x\n\n",pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5],pkt[6],pkt[7]);
	mei_my_serial.write( pkt, sizeof(pkt)) ;         //Send Command
	string mei_rply1 = "";
	mei_rply1 = mei_getresponse();
	mei_ack(pkt, sizeof(pkt));
	return;
}
/*
==================================================================================================================================
End of Return Document to customer
==================================================================================================================================
Stack Document                          NOT TESTED YET !!!!
==================================================================================================================================
*/
public:

void mei_stack_documents(void){

	printf("MEI Stack Bill called\n");
		mei_reset();                                    //Reset the MEI
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
		mei_ack(pkt, sizeof(pkt));						 //Send an Ack Packet
		mei_rply1 = "";
		mei_rply1 = mei_getresponse();					 //Get Response from MEI
	    mei_poll_response = mei_poll();                  // Poll MEI for response to command
	return;
}
/*
===============================================================================================================================
End Of Stack Document
===============================================================================================================================
Clean ascii only string
===============================================================================================================================
*/
private:

string mei_clean_string(string incomming, int lenofincomming)

{
	string outgoing ={0};

				int charcount = 0;
				int currentcount = 0;
				cout<<"Mark is cool!"<<endl;
				while (charcount < lenofincomming)
				{
					if (!isprint(incomming[charcount]))
					{
						outgoing[currentcount] = incomming[charcount];
						currentcount++;
						cout<<outgoing<<endl;
					}

				     charcount++;
	            }

	return(outgoing);
}
/*
===============================================================================================================================
End of Clean String
===============================================================================================================================
 */
};
/*
===============================================================================================================================
End of MEI Class
===============================================================================================================================
 */




















