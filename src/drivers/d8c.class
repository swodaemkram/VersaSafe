/*
	Module: d8c.class
	Author: GRC
	Created: 1-25-2019

	This module is the driver for the UTD
	This module is HEAVILY based upon the D8c.java driver by Wagner


*/


#include <time.h>
#include <unistd.h> // for bzero
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SerialStream.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <bits/stdc++.h>    // for strcpy
#include <array>            // for array copy
#include <iostream>
#include <vector>

#include "d8c.class.h"
#include "../hdr/global.h"
#include "../trim.inc"
#include "../logs.inc"

int  inventory[] =  {0, 0, 0, 0, 0, 0, 0, 0};


using namespace std;
using namespace LibSerial;



/*
    class to calcuate CRCXOR
*/
class CRCXOR
{

//friend d8c;

private:

char sum=0x0;

public:
    char getValue(void)
    {
        return sum;
    }

    void reset(void)
    {
        sum = 0x0;
    }


	// this fn is overloaded so that you can pass a c-string or char[] string
	void update(char * b, int off, int len)
	{
		for (int i=off; i< off+len; i++)
		{
			update(b[i]);
		}
	}

    void update(string b, int off, int len)
    {
//printf("CRC:");
        for (int i=off; i< off+len; i++)
		{
            update( b[i]);
//			printf("%02X:",b[i] & 0XFF);
		}
//printf("\n");
    }

    void update(char b)
    {
        sum ^= b;
    }

}; // end of crc class



string d8c_driver_version="1.0";


    string DEVHELP =
        "<u><b>D8C Dispenser</b></u><br>"
        "<br>"
        "<u>General Information:</u><br>"
        "* The D8C device <b>must</b> have all dip switches in the OFF position or the inventory counts<br>"
        "will be reset to zero every time the device restarts/power cycles.<br>"
        "* If any 'empty' plastic tube is dispensed the device will not detect the tube fall and it<br>"
        "will <b>not</b> report the tube as dispensed, but the device will decrement the inventory for<br>"
        "the column in question -- this seems to be an intentional way to allow detection of empty tybes.<br>"
        "<br>"
        "<u>Supported Command Actions:</u><br>"
        "<b>dispense</b> - Dispenses tubes using counts in dispense.in<br>"
        "<b>dispenseX</b> - Dispenses one tube from column number X (ie. dispense5 )<br>"
        "<b>dispenseamt</b> - Dispenses tubes to user according to amount in dispenseamt.in<br>"
        "<b>enable</b> - Enables load mode (so it can detect tubes being inserted)<br>"
        "<b>disable</b> - Disables load mode (so it no longer watches for tubes -- default state)<br>"
        "<b>unload</b> - Unload tubes on column indicated in unload.in<br>"
        "<b>unloadX</b> - Unload tubes from column number X (ie. unload5 )<br>"
        "<b>reset</b> - Resets device (Takes about 10 seconds)<br>"
        "<br>"
        "<u>Inputs:</u><br>"
        "<b>name.dispense.in</b> - Space separated list of tubes to dispense (ie. 1 2 3 4 5 6 7 8)<br>"
        "<b>name.dispenseamt.in</b> - Penny Amount to dispense to user (ie. 12300)<br>"
        "<b>name.values.in</b> - List of column penny value denominations space separated (i.e. 100 500 ...)<br>"
        "<b>name.counts.in</b> - List of column tube counts space separated (ie. 10 5 300 0 0 0 0 0)<br>"
        "<b>name.unload.in</b> - Column number to unload (ie. 5)<br>"
        "<br>"
        "<u>Outputs:</u><br>"
        "<b>name.status.out</b> - Output of device status TEXT. Possible values are:<br>"
        "&nbsp;&nbsp;<b>OPENING</b> - Opening port and searching for device<br>"
        "&nbsp;&nbsp;<b>NOTFOUND</b> - Device was not found/stopped communicating<br>"
        "&nbsp;&nbsp;<b>DISPENSING</b> - Device is dispensing tubes (to user)<br>"
        "&nbsp;&nbsp;<b>LOADING</b> - Device is in loading mode (waiting for tubes to be inserted)<br>"
        "&nbsp;&nbsp;<b>UNLOADING</b> - Device is in unloading a column (to user)<br>"
        "&nbsp;&nbsp;<b>IDLE</b> - Device is idle and ready to take commands<br>"
        "&nbsp;&nbsp;<b>RESETTING</b> - Device is resetting (when reset command is used)<br>"
        "&nbsp;&nbsp;<b>ALIGNING</b> - Device is aligning its motors (happens after power up)<br>"
        "<b>name.serial.out</b> - Unit's serial number<br>"
        "<b>name.fwver.out</b> - Unit's firmware version<br>"
        "<b>name.dispenses.out</b> - Space separated list of dispense counts (ie. 1 2 3 4 5 6 7 8)<br>"
        "<b>name.dispensedX.out</b> - Items dispensed from column X (1 to 8) on last dispense<br>"
        "<b>name.dispensedamt.out</b> - Amount dispensed on last dispense request (penny amount)<br>"
        "<b>name.failreason.out</b> - Reason for fail on DISPENSEFAIL, UNLOADFAIL and ERROR events<br>"
        "<b>name.inventory.out</b> - Space separated list of inventory counts (ie. 1 2 3 4 5 6 7 8)<br>"
        "<b>name.column.out</b> - Column number for tube LOAD event (also reported on other events) <br>"
        "<b>name.unloadcount.out</b> - Number of tubes detected during unload (reported on UNLOAD events)<br>"
        "<b>name.errstr.out</b> - (Compatibility only) Reports same as failreason.out on ERROR events<br>"
        "<br>"
        "<u>Events:</u><br>"
        "A single list is configured, each item consists of an event type and any action(s) to be executed.<br>"
        "When an event is triggered all actions listed with that event will execute in the listed order.<br>"
        "<b>DISPENSE</b> - Dispense successfully completed<br>"
        "<b>DISPENSEFAIL</b> - Dispense error, check failreason and dispensed outputs for details<br>"
        "<b>UNLOAD</b> - Unload successfully completed, check unloadcount.out for number of tubes detected<br>"
        "<b>UNLOADFAIL</b> - Unload error, check failreason.out and unloadcount.out output for details<br>"
        "<b>PARTIALDISPENSE</b> - Dispense is in progress, outputs are updated with current dispensed values<br>"
        "<b>PARTIALUNLOAD</b> - Unload is in progress, unloadcount is updated with current unload count<br>"
        "<b>LOAD</b> - A tube was loaded into the column indicated in name.column.out<br>"
        "<b>INVENTORY</b> - Inventory counts were updated (reported on startup and after load/dispense)<br>"
       "<b>BOOTDISPENSE</b> - Dispense detected during boot of device. Look at dispense outputs for information.<br>"
        "<b>BOOTDISPENSEFAIL</b> - Failed dispense detected at boot of device -- tube drop not detected<br>"
        "<b>BOOTUNLOAD</b> - Unload detected on boot of device. Check unloadcount and column outputs for details<br>"
        "<b>PARTIALBOOTUNLOAD</b> - Unload in progress during boot of device, Check unloadcount and column outputs<br>"
        "<b>ERROR</b> - During dispense this event reports column and failreason/errstr outputs with error details<br>"
        "<br>";



char send_char;
string fwver;
char boxaddr=0;
char devaddr=0;
char seqnum=0;


// privates
void mssleep(int micros);
void SendChar(char ochr);
//void SendString(string str);
void SendACK(void);
string DispenseOne(int dispnumber);
void Send(string Data);
bool CheckPort(string portname);
void DispenseX(string column);
void Dispense(string columnlist);
void Reset(void);
void ProcessStatus(string bb, bool update);
//void UnloadX(string column);
void UnloadColumn(int col);
void printhexB(char * bytes, int bsize, bool sent);
string char_to_string(char *buff, int len);


// publics
string GetFirmwareVersion(void);
void Dispense(string columnlist);
void DispenseX(string column);
int EnableLoad(void);
int DisableLoad(void);
void Unload(string columns);
void UnloadX(string column);
void Reset(void);
void DispenseAmt(string amt);
//void ProcessStatus(string bb, bool update);
bool Detect(void);
void Setup(void);
void DoSetup(void);
int * GetInventory(void);



// general call available to all
string Get_D8c_Driver_Version(void)
{
    return d8c_driver_version;
}



string portname;

SerialStream my_serial;
bool detected=FALSE;

/*
char PKT_STH = 0x01;
char PKT_STX= 0x02;
char PKT_ETX = 0x03;
*/

class d8c
{



public:
		d8c(string pname)
		{
			portname=pname;
			Connect();
		}

		~d8c(void)
		{
            if (my_serial)
                my_serial.Close();
		}



		int Connect(void)
		{
            my_serial.Open(portname);
printf("CONNECTING TO UTD\n");
            if (my_serial.good() )
            {
printf("CONNECTED TO UTD\n");
                my_serial.SetBaudRate(SerialStreamBuf::BAUD_19200);
                my_serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
                my_serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
                my_serial.SetParity(SerialStreamBuf::PARITY_NONE);
                my_serial.SetNumOfStopBits(1);
                DoSetup();
                return (1);
            }

        return (0);

		}

		bool CheckPort(string portname)
		{

					if ((portname.find("/dev/ttyS") != -1) || (portname.find("/dev/ttymxc") != -1) ||	(portname.find("/dev/ttyUSB") != -1) || (portname.find("/dev/ttyACM") != -1 ) )
					return TRUE;
					else
					return FALSE;

		}



void DoSetup(void)
{
	string resp;
	Send(MSG_SEARCH);
	resp=GetResponse();

	if (resp!="" && resp.length()>=7 && resp[0]==RPL_SEARCH)
	{
		detected=TRUE;

	}
	Setup();
}


void Setup(void)
{
printf("SETUP\n");
	char b[10];
	string resp;
	// stop search
	Send( MSG_SEARCHING );
	mssleep(100);

	// get version
	string tmp;
	send_char=CMD_VERSION;
	SendChar( send_char);
	resp=GetResponse();
//printhexB(resp,resp.length(),0);
	if (resp !="" && resp.length()==5 && resp[0]==RPL_VERSION)
	{
		char buffer[20];
		sprintf(buffer,"UTD v %c.%c.%c",resp[2] ,resp[3] ,resp[4] );
//		fwver= "UTD v " + resp[2] + "." + resp[3] + "." + resp[4];
		fwver=string(buffer);
	}

printf("FWVER: %s\n",fwver.c_str());


	// request initial status (also returns inventory[])
	send_char=CMD_STATUS;
	SendChar(send_char);
	resp=GetResponse();
	if (resp!="" && resp.length()>=15 && resp[0]==RPL_STATUS)
	{
		ProcessStatus(resp,TRUE);
	}

}


int * GetInventory(void)
{
	return &inventory[0];
}



// returns the firmware version string
string GetFirmwareVersion(void)
{
	return fwver;
}





/*
	Dispense ONE tube from each column in the list
	columnlist Space separated list of tubes to dispense (ie. 1 2 3 4 5 6 7 8)
*/

void Dispense(string columnlist)
{
	vector <string> cols=split(columnlist," ");
	int col;

	for (int n=0; n< cols.size(); n++)
	{
		col=stoi(cols[n]);
		DispenseOne(col);	// returns a string
	}
}


/*
	dispense ONE tube from column X
*/

void DispenseX(string column)
{
	int col=stoi(column);
	DispenseOne(col);	// returns a string
}

/*
	name.dispensein		space separated list of tubes to dispense
	name.dispenseamt	penny amount to dispense to user (ie 12300)
	name.values			list of column penny value denominations space separated (100 500 ...)
	name.counts			list of column tube counts space separated (10 5 300 0 0 0 0 0)
	name.unload			column number to unload

	name.status.out		current status
	name.serial.out		unit's serial number
	name.fwver.out		unit's firmwware version
	name.dispense.out	space separate list of dispense counts (1 2 3 4 5 6 7 8)
	name.dispenseX.out	items dispensed from column x (1 to 8) on last dispense
	name.dispenseamt.out	amount dispensed on last dispense req (penny amt)
	name.failreason.out	reason for fail on DISPENSEFAIL, UNLOADFAIL and ERROR
	name.inventory.out	space sepearated list of intentory counts (1 2 3 4 5 6 7 8)
	name.column.out		column number for tube LOAD event
	name.unloadcount.out	number of tubes detected during unload
	name.errstr.out		reports same as failreason.out on ERROR


*/
void DispenseAmt(string amt)
{
}


// enable load mode
int EnableLoad(void)
{
	string resp;
    send_char=CMD_LOAD;
    SendChar(send_char);
	resp=GetResponse();
}


// disable load mode
int DisableLoad(void)
{
	string resp;
    send_char=CMD_LOADEND;
    SendChar(send_char);
	resp=GetResponse();
}

// cols are zero based
void Unload(string columns)
{
	int scols[8];

	// convert the string to integers
	vector <string> cols=split(columns," ");
	for (int n=0; n< cols.size();n++)
	{
		scols[n]=stoi(cols[n]);
		UnloadColumn(scols[n]);
	}

}

// unload a single column
void UnloadX(string column)
{
	int col=stoi(column);
	UnloadColumn(col);
}

void Reset(void)
{
	printf("Resetting UTD\n");
	send_char=CMD_RESET;
	SendChar(send_char);
	mssleep(10000);
}







void ProcessStatus(string b, bool update)
{

	int status=b[1];

	switch(status)
	{
	case STS_LOADING:
		break;
	case STS_DISPENSING:
		break;
	case STS_UNLOADED:
		SendACK();
		break;
	case STS_UNLOADING:
	case STS_UNLOADING+1:
    case STS_UNLOADING+2:
    case STS_UNLOADING+3:
    case STS_UNLOADING+4:
    case STS_UNLOADING+5:
    case STS_UNLOADING+6:
    case STS_UNLOADING+7:
    case STS_UNLOADING+8:
		// if unload count not set or if unload count changed

		break;
        case STS_UNLOAD_COMPLETE:
			// b[4] = number of tubes unloaded
			// b[1] = column unloaded
			SendACK();
			break;
        case STS_ALIGNING:
            break;
        case STS_DISPUNVERIFIED:
            // Send acknowledgement
			SendACK();
			break;
        case STS_DISPENSED:
//			int col = b[2];
			SendACK();
            // no break
        case STS_IDLE:
        case STS_DISPDELAY:

		break;
	}	// end switch


		// get inventory
        int newinv[] = {0, 0, 0, 0, 0, 0, 0, 0};
        newinv[0] = (b[6]&0xFF)>>4;
        newinv[1] = (b[6]&0xF);
        newinv[2] = (b[7]&0xFF)>>4;
        newinv[3] = (b[7]&0xF);
        newinv[4] = (b[8]&0xFF)>>4;
        newinv[5] = (b[8]&0xF);
        newinv[6] = (b[9]&0xFF)>>4;
        newinv[7] = (b[9]&0xF);

		for (int c=0; c<8;c++)
		{
			if (inventory[c] != newinv[c])
			{
				inventory[c] = newinv[c];
				update=TRUE;
			}
		}


}





bool Detect(void)
{
	string resp;
	for (int n=0; n< 8; n++)
	{
		send_char=MSG_SEARCH[n];
		SendChar(send_char);
	}
	resp=GetResponse();
	// see if we got the expected response
	if (resp !="" && resp.length()>=7 && resp[0]==RPL_SEARCH)
	{
		detected=TRUE;
	}

	return TRUE;
}



/*
    read a string from the port
*/


string GetResponse()
{
	string resp;
//    const int BUFFER_SIZE = 150 ;
#define BUFFER_SIZE 150
	int timecount=0;
	int retrycount=0;
	int timeout=300;
	int size=0;	// expected size of input data
    char next_char;
	int char_pos=0;	// will be the next write slot in input_buffer (therefore is also count of chars in buffer)
    char input_buffer[BUFFER_SIZE];
    bzero(input_buffer,BUFFER_SIZE);
CRCXOR crc;// = new CRCXOR();


	while (timecount <= timeout && retrycount<3)
	{

	try {
			if (char_pos==0)
			{
			    while (my_serial.rdbuf()->in_avail() >0)
			    {
					timecount=0;
			        my_serial.get(next_char);
					if (next_char == (PKT_STH & 0xFF))
					{
printf("GOT PKT_STH\n");
						input_buffer[char_pos++]=PKT_STH;
						break;
					}
			        mssleep(1);
			    }//while
			}//endif

		// we have a sync char and are awaiting rest of pkt
		if (char_pos != 0)
		{


//printf("size: %d     char_pos: %d\n",size,char_pos);
                while (my_serial.rdbuf()->in_avail() >0)
                {
					timecount=0;
					my_serial.get(next_char);
					input_buffer[char_pos++]=next_char;

				}//while

				// set size when we can
				if (size == 0 && char_pos>=2)
				{
					size = (int) (input_buffer[1] & 0xFF)+1;
				}

//printf("here3\n");
			// if we got a full packet
			if (size >= 10 && char_pos >= size)
			{
printf("got a full packet (%d)\n",size);
				// check CRC

				crc.reset();
				crc.update(input_buffer,0,char_pos-1);

//char calccrc=crc.getValue();
//char pktcrc= input_buffer[char_pos-1];
//printf("calc crc: %02X   pktcrc: %02X  char_pos:%d \n",calccrc & 0xFF ,pktcrc &0xFF ,char_pos);

/*
string resp1;
resp1.resize(char_pos);
for (int x=0; x<char_pos;x++)
    resp1[x]=input_buffer[x];


printhexB(resp1,char_pos,0);
*/

				if (input_buffer[char_pos-2]==PKT_ETX && crc.getValue()== input_buffer[char_pos-1])
				{
printf("CRC was good\n");
					boxaddr=input_buffer[2];
					devaddr=input_buffer[3];
//printf("boxaddr:%02X  devaddr:%02X\n",boxaddr,devaddr);
					// if it's a search response
					if (input_buffer[1]== 0x0F && input_buffer[7]==0x3E)
						seqnum=0;	// reset sequence number
					else
						seqnum++;

					// return data bytes only
					// copy char_pos-2 bytes from input_buffer[7] to resp[0]
					resp.resize(char_pos);
					for (int x=0; x<char_pos-2;x++)
						resp[x]=input_buffer[7+x];

					break;
				}
				else
				{
printf("CRC was bad\n");
					// structure or CRC error, start over
					resp=""; size=0;char_pos=0;
				}
			}// if (size >= 10 && chars_rcvd >= size)

		}//if (char_pos!=0)


      }// end try

        catch (const std::exception& e)
		{
            char buffer[100];
            string tmp=e.what();
			if (tmp=="Input/output error in nativeavailable")
			{
	            sprintf(buffer,"%s",tmp.c_str());
    	        WriteSystemLog(buffer);
				retrycount=3;
			}
			resp=""; size=0;char_pos=0;
		}
		mssleep(10);
		timecount+=10;
		// if we didn't get a full packet within allowed time (timeout)
		if (timecount > timeout && detected)
		{
			// reset result
			resp=""; size=0;char_pos=0;
			// we can retry 3 times
			if (retrycount<2)
			{
				retrycount++;
				send_char=0;
				SendChar(send_char);
				timecount=0;	// reset everything for the retry
			}
		}


	}// end while

printhexB(resp,resp.length(),0);


//    int len=strlen(input_buffer);
//    printf("Rcvd: %s --%d chars, last char: %02X\n",input_buffer,len,next_char);
    return resp;

}



private:

// sleep micros MS

void mssleep(int micros)
{
   usleep(micros *1000);
}



void SendChar(char ochr)
{
	string tmp(1,0);
	tmp[0]=ochr;
	Send(tmp);
    mssleep(1);

}


/*
	send a string of data
	format it into a packet and send
	STH
	datalen (len of data)
	boxaddr
	devaddr
	seqHI
	seqLO
	STX
	data
	ETX
	CRC

// DATA: 3A 0F F0 00 00 00 00
// PKT: 01 07 AA AA AA AA 02 3A 0F F0 00 00 00 00 03
//      0  1  2              7


*/
void Send(string Data)
{
	int n;
	string pkt(Data.length()+9,0);
// instantiate our CRC calculator
CRCXOR crc;// = new CRCXOR();


	if (Data!="")
	{
		pkt[0]=PKT_STH;		// start of header SOH
		pkt[1] = pkt.length() -1;	// len from SOH TO EOT inclusive
		// if not search cmd, prepare data
		if (Data[0]!=0x3A)
		{
			pkt[2]= boxaddr | 0x80;
			pkt[3] = devaddr;
			pkt[4] = seqnum >> 8;
			pkt[5] = seqnum & 0xFF;
		}
		else
		{
			// search command, we use fixed values
			pkt[2]=0XAA;	// box
			pkt[3]=0xAA;	// dev
			pkt[4]=0xAA;	// seq HI
			pkt[5]=0xAA;	// seq LO
		}
		pkt[6]= PKT_STX;						// start of data

		for (n=0; n< Data.length();n++)			// insert the data
			pkt[7+n]=Data[n];
		pkt[pkt.length()-2]=PKT_ETX;			// mark end of data EOT

		crc.reset();							// zero out CRC
		crc.update(pkt,0,pkt.length()-1);		// calc new CRC
		pkt[pkt.length()-1] = crc.getValue();	// insert new CRC byte into pkt

	try	{

			printhexB(pkt,pkt.length(),1);
			SendString(pkt);
		}
		catch (const std::exception& e)
		{
			char buffer[100];
			string tmp=e.what();
            sprintf(buffer,"%s",tmp.c_str());
			WriteSystemLog(buffer);
		}

	}// endif

}




void printhexB( string bytes, int bsize, bool sent)
{

	if (sent)
		printf("SENT: ");
	else
		printf("RCVD:");

    for (int n=0; n< bsize; n++)
        printf("%02X:",(unsigned char) bytes[n]);
	printf("[%d (%02X)]\n", bsize,bsize);
}






void SendString(string str)
{
    my_serial << str;

}


void SendACK(void)
{
	send_char=CMD_ACK;
	SendChar(send_char);

}



string DispenseOne(int column)
{
	char cmd[50];
	sprintf(cmd,"%d,%d,ASCENT", CMD_DISPENSE, (0x80+column+1));
	string resp;
	long timestart=tseconds;
	string failreason;
	int dcount=0;
	bool update;

	SendString( (string) cmd );
	resp=GetResponse();
	if (resp!="" && resp.length()>0 && resp[0]==RPL_ACK)
	{
		// keep polling status, keep track of attempts
		bool done=FALSE;
		while (!done)
		{
			// poll status
			send_char=CMD_STATUS;
			SendChar(send_char);
			resp=GetResponse();
			if (resp != "" && resp.length()>=15)
			{
				switch( (long) resp[1])
				{
				case STS_DISPENSED:
					// ACK status
					SendACK();
					break;
				case STS_DISPUNVERIFIED:
					SendACK();
					failreason="Unverified dispense";
					done=TRUE;
					break;
				case STS_JAM:
					failreason="JAM detected";
					done=TRUE;
					break;
				case STS_LOADING:
				case STS_DISPENSING:
					break;
				default:
					failreason="Unexpected status " + string (1,resp[1]) +" during dispense";
					done=TRUE;
					break;
				} //end switch


				// check for timeout
				if (tseconds >= timestart+30)
				{
					failreason="Timeout";
					done=TRUE;
				}


			}
			else
			{
				// no repsonse is normal during displense 9up to a limit)
				dcount++;
				if (dcount>=3)
				{
					failreason="No response during dispense";
					break;
				}
			} // endif


			// get inventory
			int newinv[9]={0,0,0,0,0,0,0,0};
			newinv[0] = (resp[6] &0xFF) >>4;
			newinv[1] = (resp[6]&0xF);
           	newinv[2] = (resp[7] &0xFF) >>4;
            newinv[3] = (resp[7]&0xF);
            newinv[4] = (resp[8] &0xFF) >>4;
            newinv[5] = (resp[8]&0xF);
            newinv[6] = (resp[9] &0xFF) >>4;
            newinv[7] = (resp[9]&0xF);

			// check if inventory changed (assume it didn't chg until we check it)
			bool update=FALSE;
			for (int c=0; c<8; c++)
			{
					if (inventory[c] != newinv[c])
					{
						for (int x=0; x<8;x++) inventory[x]=newinv[x];
						update=TRUE;
						break;
					}
			}
			if (update)
			{
				// inventory was updated, report it
			}
			mssleep(100);	// poll delay

		}	// end while
	}
	else
	{
			// no ACK recvd
			failreason="Dispense rejected";
	} //endif

	return failreason;
} // end DispenseOne()


// INPUTS:
// col: 0-7
// col: 8 = UNLOAD_ALL

void UnloadColumn(int col)
{
printf("UnloadColumn\n");
	string resp;
	string failreason;
	int unloadcount=0;
	string strcmd;

    char cmd[50]={ (char) CMD_UNLOAD, (char)(0x80+col+1),'A','S','C','E','N','T'};
	// CMD_UNLOAD=0x87


	strcmd=char_to_string(cmd,strlen(cmd));
//printhexB(strcmd,strcmd.length(),1);
//SENT: 87:2C:82:2C:41:53:43:45:4E:54:[10 (0A)]


	Send( strcmd);
	resp=GetResponse();

	// if we got ACK
	if (resp!="" && resp.length()>0 && resp[0]==RPL_ACK)
	{
printf("we got ACK\n");
		// keep polling status, ,keep track of changes
		bool done=FALSE;
		int nocount=0;
		long timestart =tseconds;	// get our global seconds counter
		while (!done)
		{
			// poll status
			send_char=CMD_STATUS;
			SendChar(send_char);
			resp=GetResponse();

			// if response is valid
			if (resp !="" && resp.length()>=15)
			{
				// if tube detected count is different than we have
				if (unloadcount != (int) resp[4])
				{
					// get count of tubes detected
					unloadcount= (int) resp[4];
				}

			// according to status
printf("SWITCH: %d (%02X)\n",(unsigned char) resp[1],(unsigned char) resp[1]);
			switch( (unsigned char) resp[1])
			{
			case STS_UNLOADED:
				// ACK status
				SendACK();
				break;
			case STS_UNLOAD_COMPLETE:
printf("ACKing UNLOAD complete\n");
               // ACK status
				SendACK();
                done=TRUE;
                break;
			case STS_JAM:
				failreason="JAM detected";
				done=TRUE;
				break;
			case STS_UNLOADING:
            case STS_UNLOADING+1:
            case STS_UNLOADING+2:
            case STS_UNLOADING+3:
            case STS_UNLOADING+4:
           	case STS_UNLOADING+5:
           	case STS_UNLOADING+6:
           	case STS_UNLOADING+7:
           	case STS_UNLOADING+8:
printf("UNLOADING %d (%02X)\n",resp[1],resp[1]);
				// reset no response count (since we got a valid response)
				nocount=0;
				break;
			default:
				// return fail reason
				failreason="Unexpected status " + string(1,resp[1]) + " during unload";
				done=TRUE;
				break;

			} // end switch
				// check for timeout (200s)

			if (tseconds >=timestart+200)
			{
				failreason="Timeout";
				done=TRUE;

			}
		}//if (resp !="" && resp.lengh()>=15)
		else
		{
			// no response is normal during unload (up to a limit)
			nocount++;
			if (nocount>=3)
			{
				failreason="No response during unload";
				break;
			}
		}


		} //end while
           // get inventory
            int newinv[9]={0,0,0,0,0,0,0,0};
            newinv[0] = (resp[6] &0xFF) >>4;
            newinv[1] = (resp[6] &0xF);
            newinv[2] = (resp[7] &0xFF) >>4;
            newinv[3] = (resp[7] &0xF);
            newinv[4] = (resp[8] &0xFF) >>4;
            newinv[5] = (resp[8] &0xF);
            newinv[6] = (resp[9] &0xFF) >>4;
            newinv[7] = (resp[9] &0xF);

printf("INVENTORY\n");
for (int n=0; n<8;n++)
	printf("COL %d: %d\n",n,newinv[n]);

            // check if inventory changed (assume it didn't chg until we check it)
            bool update=FALSE;
            for (int c=0; c<8; c++)
            {
                    if (inventory[c] != newinv[c])
                    {
                        for (int x=0; x<8;x++) inventory[x]=newinv[x];
                        update=TRUE;
                        break;
                    }

			}
			// if inventory was updated, report it
			if (update)
			{
			}
			// poll delay
			mssleep(100);


	}
	else
	{
			// no ACK rcvd
			failreason="Unload rejected";
	}
		// report final number of tubes detected
		//unloadcount

		// create an unload action
		if (failreason=="")
		{
		}

	// idle again

} // end UnloadColumn()





/*
	counts =  Space separated list of tubes to dispense (ie. 1 2 3 4 5 6 7 8)

// dispense tubes from provided counts
*/

void DoDispense(string counts, bool GetValues)
{
	#define NOVAL -1
	int disp[8];
	int cnts[8];
	int vals[8]={NOVAL,NOVAL,NOVAL,NOVAL,NOVAL,NOVAL,NOVAL,NOVAL};	// vals for each column
	int damt=0;		// dispensed amt
	string failreason="";
	bool hasmore=FALSE;
	string dispenses;

	vector <string> mycounts=split(counts," ");

	// failed indexes (to conrol dispense attempts during incomplete dispense)
	bool fi[8]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};

	// reset initial values
	for (int i=0; i <8; i++)
	{
		disp[i]=0;
		cnts[i]=stoi(mycounts[i]);
	}

	// for each dispenser
	for (int d=0; d<8; d++)
	{
		// count successful dispenses
		int sd=0;
		// get count to dispense
		int c =cnts[d];
		while (c>0 && !fi[d])
		{
			// try to dispense one from this column
			failreason = DispenseOne(d);
			// if success
			if (failreason=="")
			{
				c--;	// decrease count
				sd++;	// increase successful dispense count
				// add to dispensed amount
				if (vals[0] != NOVAL)
					damt += vals[d];
			}
			else
			{
				fi[d] = TRUE;	// set column as failed
				//TODO log error
			}

			// if we got values while dispensing
			if (vals[0]==NOVAL)
			{
				for (int i=0; i<8; i++)
					vals[i]=0;
				for (int x=0;x<8;x++)
				{
					vals[x]=cnts[x];
				}
			}
		} // end while

		hasmore=FALSE;
		dispenses="";
		for (int i=0;i<8;i++)
		{
//TODO			
		}

	}// end for

	// check if any column failed and if so, set reason as "incomplete dispense"
	failreason="";
		for (int i=0;i<8;i++)
		{
			if (fi[i])
				failreason="incomplete displense";
		}

		// create dispense action
		//TODO

}


// convert a char string to a c-string
string char_to_string(char *buff, int len)
{
	string resp(len,0);
//	resp.resize(len,0);
	for (int x=0; x<len; x++)
		resp[x]=buff[x];
}




};//end of class









