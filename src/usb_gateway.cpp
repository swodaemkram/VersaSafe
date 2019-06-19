
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
#include "config.inc"

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
mei * validator_left = NULL;
mei * validator_right = NULL;
//ucd_val * validator_ucd = NULL;
//int init_mei(void);
int init_validators(void);
//void get_mei_serialNumber(void);
//char Logbuff[50];
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
    if(cfg.validator_left)
	{
	printf("deleting validator\n");
	delete validator_left;
	}

    if(cfg.validator_right)
    {
    printf("deleting validator\n");
    delete validator_right;
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
// 					 Validator Section
//=================================================================================================

//-------------------------------------------------------------------------------------------------
//Method to Initialize the  Validator
//-------------------------------------------------------------------------------------------------

int init_validators(void)
{

	if(cfg.validator_left)
	{
		printf("\nInitializing Validator_Left ....\n");
		sprintf(Logbuff,"Validator_Left Initialized");
		WriteSystemLog(Logbuff);//log that this was called
		mei * validator_left = new mei("/dev/ttyUSB0",1);
		printf("\nValidator_Left  Initialized!\n");
	}

	if(cfg.validator_right)
	{
		printf("\nInitializing Validator_Right ....\n");
		sprintf(Logbuff,"Validator_Right Initialized");
		WriteSystemLog(Logbuff);//log that this was called
		mei * validator_left = new mei("/dev/ttyUSB1",1);
		printf("\nValidator_Right Initialized!\n");
	}

	if(cfg.validator_ucd)
	{
		printf("\nInitializing Validator_ucd ....\n");
		sprintf(Logbuff,"Validator_ucd Initialized");
		WriteSystemLog(Logbuff);//log that this was called
		mei * validator_ucd = new mei("/dev/ttyUSB2",1);
		printf("\nValidator_ucd Initialized!\n");
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------
//MEI Poll Method
//-------------------------------------------------------------------------------------------------------
void MEIpoll(void)
{
	if(cfg.validator_left)validator_left->mei_poll();
	if(cfg.validator_right)validator_right->mei_poll();
}
//-------------------------------------------------------------------------------------------------------
// NEW Functions
//-------------------------------------------------------------------------------------------------------
// Validator verify method (we turn on all validators here)
//-------------------------------------------------------------------------------------------------------
string Validator_verify(void)
{

	if(cfg.validator_left)
	{
		validator_left->Rx_Command("verify", 0, 0, 1, 0);
	}

	if(cfg.validator_right)
	{
		validator_right->Rx_Command("verify", 0, 0, 1, 0);
	}

	if(cfg.validator_ucd)
	{
		//validator_ucd->Rx_Command("verify", 0, 0, 1, 0);// TODO Need to enable this when ucd driver is completed

	}

	return "test";
}
//-------------------------------------------------------------------------------------------------------
// End of Validators Verify
//-------------------------------------------------------------------------------------------------------
// Validator Stack Method
//-------------------------------------------------------------------------------------------------------
string Validator_stack(int which)
{

	if(cfg.validator_left)
	{
		validator_left->Rx_Command("stack", 0, 1, 0, 0);
	}

	if(cfg.validator_right)
	{
		validator_right->Rx_Command("stack", 0, 1, 0, 0);
	}

	if(cfg.validator_ucd)
	{
		//validator_ucd->Rx_Command("stack", 0, 1, 0, 0); //TODO Need to add this when ucd driver is completed

	}

	return "test";
}
//-------------------------------------------------------------------------------------------------------
// End of Validator Stack Method
//-------------------------------------------------------------------------------------------------------
// Validator Model Method
//-------------------------------------------------------------------------------------------------------
string Validator_model(int which)
{
	if (which == 0  && cfg.validator_left == true)
	{
		validator_left->Rx_Command("model", 1, 0, 0, 0);
	}

	if (which == 1  && cfg.validator_right == true)
	{
		validator_right->Rx_Command("model", 1, 0, 0, 0);
	}

	return "test";
}
//-------------------------------------------------------------------------------------------------------
// End of Validator Model Method
//-------------------------------------------------------------------------------------------------------
// Validator Info Method
//-------------------------------------------------------------------------------------------------------
string Validator_info(int which)
{
	//TODO Need to think about how I will implement this

	return "test";
}
//------------------------------------------------------------------------------------------------------
// End of Validator Info Method
//------------------------------------------------------------------------------------------------------
// Validator Reset Method
//------------------------------------------------------------------------------------------------------
void Validator_reset(int which)
{

	if (which == 0  && cfg.validator_left == true)
	{
		validator_left->Rx_Command("reset", 1, 0, 0, 0);
	}

	if (which == 1  && cfg.validator_right == true)
	{
		validator_right->Rx_Command("reset", 1, 0, 0, 0);
	}

	return;
}
//-----------------------------------------------------------------------------------------------------
// Validator Reset Method
//-----------------------------------------------------------------------------------------------------
// Validator Serial Method
//-----------------------------------------------------------------------------------------------------
string Validator_serial(int which)
{
	if (which == 0  && cfg.validator_left == true)
	{
		validator_left->Rx_Command("serial", 1, 0, 0, 0);
	}

	if (which == 1  && cfg.validator_right == true)
	{
		validator_right->Rx_Command("serial", 1, 0, 0, 0);
	}

	return "test";
}
//-----------------------------------------------------------------------------------------------------
// End of Validator Serial Method
//-----------------------------------------------------------------------------------------------------
// Validator Idle Method
//-----------------------------------------------------------------------------------------------------
string Validator_idle(int which)
{
	if (which == 0  && cfg.validator_left == true)
	{
		validator_left->Rx_Command("idle", 1, 0, 0, 0);
	}

	if (which == 1  && cfg.validator_right == true)
	{
		validator_right->Rx_Command("idle", 1, 0, 0, 0);
	}

	return "test";
}
//---------------------------------------------------------------------------------------------------
// Validator get result command
//------------------------------------------------------------------------------------------------------

  string Validator_get_results(int which)
  {
	  string the_result;
	  if (which == 0  && cfg.validator_left == true)
	  	{
	  		the_result = validator_left->get_result();
	  		validator_left->clr_get_result();
	  	}

	  	if (which == 1  && cfg.validator_right == true)
	  	{
	  		the_result = validator_left->get_result();
	  		validator_right->clr_get_result();
	  	}

	  	return the_result;
  }




//------------------------------------------------------------------------------------------------------
//End of MEI Commands
//------------------------------------------------------------------------------------------------------

#include "api_usb.inc"



