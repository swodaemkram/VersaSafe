/*
    Module: usb_gateway.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 12-14-2017
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2017 FireKing Security Group

    Version: 1.0

	This module is the interface between the business logic and the USB hardware
	locks
	definitions in hdr/usb_gateway.h
	1. outter lock
	2. inner lock
	3. shutter
	3. Side car
	4. base
*/


using namespace std;

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logs.inc"


// LOCK DRIVER
#include "drivers/apuLock.class"        // include the LOCK driver class
#include "hdr/usb_gateway.h"
#include "hdr/fire.h"

// VEND BOARD DRIVER (D8C)
//Bus 002 Device 026: ID 0403:6001 Future Technology Devices International, Ltd FT232 USB-Serial (UART) IC
#include "drivers/d8c.class"
#include "drivers/mei_driver.class"



//======================================================================================================
//Beginning of MEI Declarations
//======================================================================================================
//MEI Validator public functions
mei * validator = NULL;
int init_mei(void);
void mei_timeout_timer(void);
void get_mei_serialNumber(void);
extern char rx_packet[40];
extern char tx_packet[30];
extern char comm_port[250];
extern char MEI_CURRENT_COMMAND[30];
extern char MEI_STATUS[30];
//extern char clean_text[30];
extern char MEI_LAST_COMMAND[30];
extern char LAST_MEI_STATUS[30];
//-------------------------------------------------------------------------------------------------------
int q;
int mei_single_run_command = 0;//Flag for a single run command
int mei_stacking = 0; //Global Status Value for the MEI
int mei_verifying = 0;//Global Status Value for the MEI
int mei_timeout_timer_value = 0;//Global Timeout Timer Variable
unsigned int tx_crc = 0;
char Logbuff[50];
char last_clean_text[30] = {0};
//======================================================================================================
//End of MEI Declarations
//======================================================================================================

d8c * utd = NULL;
// public VEND functions
int Init_D8C(void);
void Reset_D8C(void);
int Disable_Load_D8C(void);
int Enable_Load_D8C(void);
int Unload_D8C(string cols);	// cols are zero based
int Unload_D8C(int column);
string Get_d8_driver(void);
int * GetUTDInventory(void);
bool D8C_detected(void);

// public lock functions
int Init_USBX(int index);
void Kill_USBX(int index);
int Lock_Lock(int index) ;
int Unlock_Lock(int index);
int SetLockSerial(int index, string ser);
string GetLockSerial(int index);
void SetLockDelay(int index, int delay);
string GetLockFirmware(int index);
string GetLockVolts(int index);
void GetLockStatus(int index);
string GetLockDriverVersion(void);
void PrintLockStats(int index);
string GetLockType(int index);
int GetLockDelay(int index);
int DownloadLockFirmware(int index);
bool GetIsLocked(int index);
void Enable_Lock(int index);

//private
void Enable_Lock(int index);

//TODO implement firmware download
//TODO implement setting serial number
//TODO implement setting lock address



//=================================================================================================
//					USB LOCK SECTION
//=================================================================================================



// this struct holds all info about a lock
struct alock
{
    apuLock * ptr=NULL;     // pointer to instantiated lock object
	bool enabled=FALSE;		// set from config.xml file
    bool connected;         // USB connection status
    bool islocked=TRUE;
    string port;            // port of connection
    string serial;          // lock serial number
    int delay=10;           // seconds
    int doorsensor;         // 1 = open
    int locksensor;         // 1 = unlocked
    string firmware;        // firmware version
    string volts;
    string locktype;        // summit or ascent
    int mtimer=0;           // lock mtimer value, rturned from getStatus()
};


// indexed by lock numbers, OUTTER_LOCK,INNER_LOCK,SHUTTER_LOCK,SIDECAR_LOCK,BASE_LOCK as defined in hdr/usb_gateway.h
alock locks[MAX_LOCKS];

//TODO -- must populate serialports[] properly via scan for locks
string serialports[]={"/dev/ttyACM0","/dev/ttyACM1","/dev/ttyACM2","/dev/ttyACM3"};


// this array maps the locks to the serialports[] array above
/*
int lock_map[MAX_LOCKS]={
					0,
					1,
					2,
					3,
					4,
					-1,-1,-1};
*/
// access as int sp_index=lock_map[OUTTER_LOCK] to get the index into serialports[]
//	port_string = serial_ports[sp_index]


/*
*/
void USB_init(void)
{
	printf("Executing::USB_init()\n");
    Init_USBX(OUTTER_LOCK);
    Init_USBX(INNER_LOCK);
    Init_USBX(SHUTTER_LOCK);
    Init_USBX(SIDECAR_LOCK);
    Init_USBX(BASE_LOCK);


	printf("USB_init() done\n");
}

void USB_shutdown(void)
{
//MARK    Kill_USBX(OUTTER_LOCK);
//MARK    Kill_USBX(INNER_LOCK);
//MARK    Kill_USBX(SHUTTER_LOCK);
//MARK    Kill_USBX(SIDECAR_LOCK);
//MARK    Kill_USBX(BASE_LOCK);

//TODO - must make contigent upon xml config
    	if( utd )
		delete utd;
//TODO - must make contigent upon xml config
	//validator->mei_shutdown();
    if(validator)
	{
	printf("deleting validator\n");
	delete validator;
	}
	return;
}



// this is called after reading config.xml in versasafe.cpp
void Enable_Lock(int index)
{
	printf("Enabling lock:%d\n",index);
    locks[index].enabled=TRUE;
}



/*
ALL OF THIS IS IN hdr/usb_gateway.h

#define MAX_LOCKS  		8
#define OUTTER_LOCK 	0
#define INNER_LOCK 		1
#define SHUTTER_LOCK	2
#define SIDECAR_LOCK	3
#define BASE_LOCK       4




*/


/*
	called from common.cpp:GeneralSetup()
	returns 1 on success
	0 on error

defined in hdr/usb_gateway.h
	index will be one of the following
	#define MAX_LOCKS  8
	#define OUTTER_LOCK     0
	#define INNER_LOCK     	1
	#define SHUTTER_LOCK    2
	#define SIDECAR_LOCK    3
	#define BASE_LOCK       4

*/

int Init_USBX(int index)
{
	char buffer[80];
	if (locks[index].enabled)
	{
printf("Init_USBX::lock: %d\n",index);
		// instantiate a lock driver
		apuLock * vault = new apuLock(serialports[index]);
		locks[index].port=serialports[index];
		locks[index].ptr=vault;
		// returns 1 on success
		int stat=vault->Connect();
		if (stat !=1)
		{
			sprintf(buffer,"Unable to connect to Lock %d",index);
printf("%s\n",buffer);
			WriteSystemLog(buffer);
		}
		locks[index].connected=stat;
		GetLockSerial(index);
		GetLockFirmware(index);
		GetLockVolts(index);
		GetLockStatus(index);
		GetLockDelay(index);
		GetLockType(index);
	}

	PrintLockStats(index);
	return 1;
}


void PrintLockStats(int index)
{
	string port=locks[index].port;
	string serial=locks[index].serial;
	string firmware=locks[index].firmware;
	bool connected=locks[index].connected;
	int delay=locks[index].delay;
	string ltype=locks[index].locktype;
	string volts=locks[index].volts;
	int mtimer=locks[index].mtimer;

	printf("\n========================================\n");
	printf("Lock index [%d]\n",index);
	printf("Enabled: %d\n",locks[index].enabled);
	printf("Port: %s\n",port.c_str() );
	printf("Serial: %s\n",serial.c_str() );
	printf("Firmware: %s\n",firmware.c_str() );
	printf("Connected: %s\n", (connected) ? "YES":"NO");
	printf("Volts: %s\n",volts.c_str());
	printf("Delay: %d seconds\n",delay);
	printf("mtimer: %d\n",mtimer);
	printf("Type: %s\n",ltype.c_str() );
    printf("========================================\n\n");

}





void Kill_USBX(int index)
{
    if (locks[index].enabled)
    {
        locks[index].connected=0;
//-GRC
        if (locks[index].ptr)
        {
            locks[index].ptr->close();
            delete locks[index].ptr;
        }
    }
}


// returns 1 on success, 0 on failure

int DownloadLockFirmware(int index)
{
	int res=0;
	if (!locks[index].enabled) return res;
    res=locks[index].ptr->downloadFirmware();
    return res;
}

/*
	returns locked status of the lock
	returns TRUE if locked, FALSE otherwise
*/
bool GetIsLocked(int index)
{
	return locks[index].islocked;
}

int GetLockDelay(int index)
{
	string delay=locks[index].ptr->getdelay();
	int del=stoi(delay);
	locks[index].delay= del;
	return del;
}


string GetLockDriverVersion()
{

	string ret= Get_Lock_Driver_Version() ;
	return ret;
}


/*
// int mtimer is set to zero by LOCK cmd (D)
// sprintf(tmp, "OK 0%o t=%d;", mtimer > 0 ? 1 : 0, mtimer);
//OK 01 denote timer is active
//t=%d gives the mtimer value
//        mtimer = sec * 40; set at E cmd and counts down in an interrupt till it expires
// so, if etime is 10, then mtimer gets set to 400 and counts down

	getStatus() returns OK 00 t=0;

*/
void GetLockStatus(int index)
{
    if (!locks[index].ptr) return;    // if not active, return

	// get the t=??? value and store as int to mtimer
	string stat=locks[index].ptr->getStatus();
	int mtimer =stoi(stat.substr(8) );
	locks[index].mtimer=mtimer;
}


string GetLockVolts(int index)
{
    if (!locks[index].ptr) return 0;    // if not active, return

	string v=locks[index].ptr->getVolts();
	locks[index].volts=v;
	return v;
}


string GetLockType(int index)
{
    if (!locks[index].ptr) return 0;    // if not active, return
	string ltype= locks[index].ptr->getLockType();
	locks[index].locktype=ltype;
	return ltype;
}


// returns 1 on success
// 0 on error

int Lock_Lock(int index)
{
    if (!locks[index].ptr) return 0;    // if not active, return

	// returns 0 on error, 1 on success
	int stat=locks[index].ptr->Lockit();
	if (stat)
			locks[index].islocked=TRUE;
	else
			locks[index].islocked=FALSE;
	return stat;
}


/*
#define OUTTER_LOCK     0
#define INNER_LOCK     	1
#define SHUTTER_LOCK    2
#define SIDECAR_LOCK    3
#define BASE_LOCK       4

*/
// returns 1 on success
// 0 on error

int Unlock_Lock(int index)
{
	printf("Unlock_Lock %d\n",index);
	if (!locks[index].ptr) return 0;	// if not active, return

    // returns 0 on error, 1 on success
    int stat=locks[index].ptr->Unlockit();
	printf("Lock unlock status: %d\n",stat);
	if (stat)
		locks[index].islocked=FALSE;
	else
		locks[index].islocked=TRUE;
    return stat;

}


string GetLockFirmware(int index)
{
    if (!locks[index].ptr) return 0;    // if not active, return

	string firm=locks[index].ptr->getFirmware();
	locks[index].firmware=firm;
	return firm;
}


string GetLockSerial(int index)
{
    if (!locks[index].ptr) return 0;    // if not active, return

	string ser=locks[index].ptr->getSerial();
	locks[index].serial=ser;
	return ser;
}

// serial number max len 31
// returns 1 on success, 0 on failure
int SetLockSerial(int index, string ser)
{
    if (!locks[index].ptr) return 0;    // if not active, return

		if (ser.length() > 31) return 0;
		int res=locks[index].ptr->setSerial(ser);
		return res;
}

void SetLockDelay(int index, int delay)
{
    if (!locks[index].ptr) return;    // if not active, return

	int del=locks[index].ptr->setdelay(delay);
	locks[index].delay=delay;
}

//=================================================================================================
// 					D8C - VEND BOARD SECTION
//=================================================================================================

/*
void DispenseX(string column);
void Enable(void);
void Disable(void);
void Unload(string columns);
void UnloadX(string column);
void Reset(void);
void DispenseAmt(string amt);
void ProcessStatus(string bb, bool update);
bool Detect(void);
void Setup(void);
*/

int Init_D8C(void)
{
printf("init_d8c\n");
		int index=0;
//        d8c * utd = new d8c(serialports[index]);
			d8c * utd = new d8c("/dev/ttyUSB0");
//utd->Reset();
	if (D8C_detected)
		GetUTDInventory();	// we can safely get inventory after setup
}


bool D8C_detected(void)
{
	//bool detected= utd->isdetected();
	return detected;
}



int Enable_Load_D8C(void)
{
	int retval;

	retval=utd->EnableLoad();
	return retval;
}


int Disable_Load_D8C(void)
{
	int retval;

	retval=utd->DisableLoad();
	return retval;
}

// unload a single column
// cols are zero based
// call with column=8 to unload all

int Unload_D8C(int column)
{

column=8;

	string cols= to_string(column);
	utd->UnloadX( cols );

//	Enable_Load_D8C();

}

int utd_inventory[] =  {0, 0, 0, 0, 0, 0, 0, 0};


int * GetUTDInventory(void)
{
printf("GetUTDInventory\n");
    if (!D8C_detected) return NULL;
	int *inv;
	inv=utd->GetInventory();

	// copy to our own struc
	for (int n=0; n<8;n++)
	{
		utd_inventory[n]=inv[n];
//		printf("utd_inv%d: %d\n",n,utd_inventory[n]);
	}

	return &utd_inventory[0];
}

int Unload_D8C(string cols)
{
}

void Reset_D8C(void)
{
	utd->Reset();
}

string Get_d8_driver(void)
{
	return Get_D8c_Driver_Version();
}

//=================================================================================================
// 					MEI Validator Section
//=================================================================================================

//-------------------------------------------------------------------------------------------------
//Command to Initialize the MEI Validator
//-------------------------------------------------------------------------------------------------
int init_mei(void)
{
		printf("\nInitializing MEI Validator ....\n");
	    sprintf(Logbuff,"MEI Validator Initialized");
	    WriteSystemLog(Logbuff);//log that this was called
	    mei * validator = new mei("/dev/ttyUSB0",1);//TODO This needs to come from the config file
		printf("\nMEI Validator Initialized!\n");
		return(0);
}
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
// Command to Get the MEI Serial Number
//---------------------------------------------------------------------------------------------------
void get_mei_serialNumber(void)
{
	printf("get_mei_serialNumber was called\n");
	sprintf(Logbuff,"get_mei_serialNumber was called");
	WriteSystemLog(Logbuff);//log that this was called
	memset(MEI_CURRENT_COMMAND,0,30); //Clear the Current Command from the polling stream
	strncpy(MEI_CURRENT_COMMAND,"serial",6);//insert the serial command into the polling stream
	mei_single_run_command = 1;// insert it only 1 time
	return;
}
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//  Command to Reset MEI Validator can take up to 10 seconds
//-----------------------------------------------------------------------------------------------------
void mei_reset_func(void)
{
    	printf("mei_reset_func was called\n");
    	sprintf(Logbuff,"mei_reset_func was called");
    	WriteSystemLog(Logbuff);//log that this was called
    	memset(MEI_CURRENT_COMMAND,0,30);//clear current command from the polling stream
    	strncpy(MEI_CURRENT_COMMAND,"reset",5);//insert reset into the polling stream
    	mei_single_run_command = 1;//insert only once
    	return;
}
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// Command to get MEI Model Number (This is just a test and will become MEI Get INFO)
//-----------------------------------------------------------------------------------------------------
string mei_getmodel_func(void)
{
	printf("mei_getmodel_func was called\n");
	string returnvalue;
	sprintf(Logbuff,"mei_getmodel_func was called");
	WriteSystemLog(Logbuff);//log that this was called
	memset(MEI_CURRENT_COMMAND,0,30);//clear current command from the polling stream
	strncpy(MEI_CURRENT_COMMAND,"model",5);//insert model into the polling stream
	mei_single_run_command = 1;//insert only once
	return(returnvalue);
}
//------------------------------------------------------------------------------------------------------
//Command to get MEI varname
//------------------------------------------------------------------------------------------------------
void mei_get_varname_func(void)
{
	printf("mei_get_varname_func was called\n");
	sprintf(Logbuff,"mei_get_varname_func was called");
	WriteSystemLog(Logbuff);//log that this was called
	memset(MEI_CURRENT_COMMAND,0,30);//clear current command from the polling stream
	strncpy(MEI_CURRENT_COMMAND,"varname",5);//insert command into the polling stream
	mei_single_run_command = 1;//insert only once
	return;
}
//------------------------------------------------------------------------------------------------------
//Command to get Boot Version
//------------------------------------------------------------------------------------------------------
void mei_get_bootver_func(void)
{
	printf("mei_get_bootver_func was called\n");
	sprintf(Logbuff,"mei_get_bootver_func was called");
	WriteSystemLog(Logbuff);//log that this was called
	memset(MEI_CURRENT_COMMAND,0,30);//clear current command from the polling stream
	strncpy(MEI_CURRENT_COMMAND,"bootver",5);//insert command into the polling stream
	mei_single_run_command = 1;//insert only once
	return;
}
//------------------------------------------------------------------------------------------------------
//Command to get app Version
//------------------------------------------------------------------------------------------------------
void mei_get_appver_func(void)
{
	printf("mei_get_appver_func was called\n");
	sprintf(Logbuff,"mei_get_appver_func was called");
	WriteSystemLog(Logbuff);//log that this was called
	memset(MEI_CURRENT_COMMAND,0,30);//clear current command from the polling stream
	strncpy(MEI_CURRENT_COMMAND,"appver",5);//insert command into the polling stream
	mei_single_run_command = 1;//insert only once
	return;
}
//------------------------------------------------------------------------------------------------------
//Command to idle the mei unit !!! This command should be given after a reply from any of
// The above commands !!!! to keep the MEI in sync
//************* I HAVE AUTOMATED THIS, SENDING AN IDLE COMMAND IS NO LONGER NECESSARY ******************
//------------------------------------------------------------------------------------------------------
void mei_idle_func(void)
{
	printf("mei_idle_func was called\n");
	sprintf(Logbuff,"mei_idle_func was called");
	WriteSystemLog(Logbuff);//log that this was called
	memset(MEI_CURRENT_COMMAND,0,30);//clear current command from the polling stream
	strncpy(MEI_CURRENT_COMMAND,"idle",5);//insert command into the polling stream
	mei_single_run_command = 1;//insert only once
	return;
}
//-----------------------------------------------------------------------------------------------------
//The commands below are Polling commands they need to be injected
//into the polling stream to get a reply that happens at the
//users time frame. The user may put a bill in one at a time
//or many all at once. To allow for this variable timing
//We continually poll until we get a response
//-----------------------------------------------------------------------------------------------------
// Command to MEI to verify Bills
//-----------------------------------------------------------------------------------------------------
void mei_verify_bill_func(void)
{
	memset(MEI_CURRENT_COMMAND,0,30);
	sprintf(Logbuff,"mei_verify_bill_func was called");
	WriteSystemLog(Logbuff);//log that this was called
	strncpy(MEI_CURRENT_COMMAND,"verify",6);
	mei_verifying = 1;//Turns on the Timeout Timer
	return;
}
//-----------------------------------------------------------------------------------------------------
// MEI stack Command
//-----------------------------------------------------------------------------------------------------
void mei_stack(void)
{
	memset(MEI_CURRENT_COMMAND,0,30);
	sprintf(Logbuff,"mei_stack was called");
	WriteSystemLog(Logbuff);//log that this was called
	strncpy(MEI_CURRENT_COMMAND,"stack",5);
	mei_stacking = 1;//Turns On The Timeout Timer
	return;
}
//------------------------------------------------------------------------------------------------------
//MEI Polling Function
//------------------------------------------------------------------------------------------------------
// !!!! The order of what happens below is VERY IMPORTANT for proper operation of the MEI Unit !!!
//                      This is the heart of the MEI communications
// This works like a  wheel every 300ms looping through and running each process to keep that data
// streaming and in sync
//------------------------------------------------------------------------------------------------------
void MEIpoll(void)
{

if (q>=3)//This divides the 100ms time slice by 3 for a 300ms time slice for polling

{

if (strcmp(rx_packet,"") == 0)//if the last RXed Packet is Blank Start a new polling session
{
	tx_packet[0] = '\x02';//stuff STX
	tx_packet[1] = '\x08' ;//Number of Bytes in the packet
	tx_packet[2] = '\x10' ;//Poll Command
	tx_packet[3] = '\x10' ;// No Data
	tx_packet[4] = '\x00' ;// No Data
	tx_packet[5] = '\x00' ;// No Data
	tx_packet[6] = '\x00' ;// Set a space for the ETX
	tx_packet[7] = '\x00' ;// Set a space for the CRC
	tx_crc = validator->do_crc(tx_packet,8);//run the crc function
	tx_packet[6] = '\x03' ;// Insert ETX into packet
	tx_packet[7] = tx_crc ;// Insert the returned CRC into packet
}
//printf("\n%02x|%02x|%02x|%02x|%02x|%02x|%02x|%02x\n",tx_packet[0],tx_packet[1],tx_packet[2],tx_packet[3],tx_packet[4],tx_packet[5],tx_packet[6],tx_packet[7]); //DEBUG CODE
//\x02\x08\x10\x1f\x14\x00 // Use this Packet template to get MEI to accept bills

//==================Start of Polling========================

if (rx_packet[2] == '\x20') //Standard Polling
{
	tx_packet[2] = '\x11';
    tx_packet[6] = '\x00';
    tx_packet[7] = '\x00';
    tx_crc = 0;
    tx_crc = validator->do_crc(tx_packet,8);
    tx_packet[6] = '\x03';
    tx_packet[7] = tx_crc;
}

if (rx_packet[2] == '\x21') //Standard Polling Ack
{
	tx_packet[2] = '\x10';
	tx_packet[6] = '\x00';
	tx_packet[7] = '\x00';
	tx_crc = 0;
	tx_crc = validator-> do_crc(tx_packet,8);
	tx_packet[6] = '\x03';
	tx_packet[7] = tx_crc;
}

if (rx_packet[2] == '\x70') //Enhanced Information Polling
{
	tx_packet[2] = '\x11';
    tx_packet[6] = '\x00';
    tx_packet[7] = '\x00';
    tx_crc = 0;
    tx_crc = validator->do_crc(tx_packet,8);
    tx_packet[6] = '\x03';
    tx_packet[7] = tx_crc;
}

if (rx_packet[2] == '\x71') //Enhanced Information Polling Ack
{
	tx_packet[2] = '\x10';
	tx_packet[6] = '\x00';
	tx_packet[7] = '\x00';
	tx_crc = 0;
	tx_crc = validator->do_crc(tx_packet,8);
	tx_packet[6] = '\x03';
	tx_packet[7] = tx_crc;
}
//==================End of Polling=============================
//===============Business  Logic for MEI=======================

validator->process_commands();           //Process In coming commands
validator->mei_tx(tx_packet, comm_port); //Transmit Packet to MEI
//printf("tx_packet = %02x|%02x|%02x|%02x|%02x|%02x|%02x|%02x|\n",tx_packet[0],tx_packet[1],tx_packet[2],tx_packet[3],tx_packet[4],tx_packet[5],tx_packet[6],tx_packet[7]);
validator->mei_rx(comm_port);            // Receive packet from MEI
//printf("rx_packet = %02x|%02x|%02x|%02x|%02x|%02x|%02x|%02x|\n",rx_packet[0],rx_packet[1],rx_packet[2],rx_packet[3],rx_packet[4],rx_packet[5],rx_packet[6],rx_packet[7]);
validator->process_response();           // What does the Response from the MEI mean

q=0;//Reset Polling Divider
if(mei_verifying == 1 || mei_stacking == 1) mei_timeout_timer();//Turn On Timeout Timer if this is a polling command
if (strcmp(MEI_LAST_COMMAND,MEI_CURRENT_COMMAND)!= 0) printf("MEI_CURRENT_COMMAND = %s\n",MEI_CURRENT_COMMAND);//Print Only Changes
if (strcmp(LAST_MEI_STATUS,MEI_STATUS) !=0)
{
	printf("current MEI_STATUS = %s\n",MEI_STATUS);//Print Only Changes
	mei_timeout_timer_value = 0; //Reset the timer
}
if (mei_single_run_command == 1)
{
	memset(MEI_CURRENT_COMMAND,0,30);
	strncpy(MEI_CURRENT_COMMAND,"idle",4); //If a single run command was sent automatically send an idle command
	mei_single_run_command = 0;
}

strcpy(MEI_LAST_COMMAND,MEI_CURRENT_COMMAND);
memset(LAST_MEI_STATUS,0,30);
strcpy(LAST_MEI_STATUS,MEI_STATUS);

}//End of polling
q++;
return;
}
//----------------------------------------------------------------------------------------------------
//MEI Timeout Timer only is enabled when a Polling command is running
//----------------------------------------------------------------------------------------------------
void mei_timeout_timer(void)
{
 time_t seconds;
 seconds = time(0);
 if (mei_timeout_timer_value == 0) mei_timeout_timer_value = seconds;
 int timeout_value =  seconds - mei_timeout_timer_value;
 //printf("%d\n",timeout_value);//DEBUG!!!

 if (timeout_value >= 30)//TODO load the timeout value from the config file
 {
	 memset(MEI_CURRENT_COMMAND,0,30);
	 sprintf(Logbuff,"MEI Time out timer just called mei_idle_func");
	 WriteSystemLog(Logbuff);//log that this was called
	 strncpy(MEI_CURRENT_COMMAND,"idle",4);
	 mei_timeout_timer_value = 0; //Reset the timer
	 mei_stacking = 0;//reset stacking flag
	 mei_verifying = 0;//reset verifying flag
 }
 return;
}

//------------------------------------------------------------------------------------------------------
//End of MEI Commands
//------------------------------------------------------------------------------------------------------

#include "api_usb.inc"



