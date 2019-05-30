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
void mei_reset_func(void);
string get_mei_driver(void);
string mei_verify_bill_func(void);
string mei_getmodel_func(void);
//string mei_get_info(void);
//void mei_enable(void);
//void mei_dissable(void);
//void mei_accept(void);
//void mei_enable_bookmark(void);
//void mei_enable_in(int meienableincount);
//void mei_accept_in(string meibillstoaccept);
//void mei_dissable_time(int meidissabletimeinsec);
//int * mei_get_inventory(void);
//void mei_connect(string pname);
//string mei_status(void);
//string mei_stack(void);

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
    Kill_USBX(OUTTER_LOCK);
    Kill_USBX(INNER_LOCK);
    Kill_USBX(SHUTTER_LOCK);
    Kill_USBX(SIDECAR_LOCK);
    Kill_USBX(BASE_LOCK);

//TODO - must make contigent upon xml config
	if( utd )
		delete utd;
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
		locks[index].ptr->close();
		locks[index].connected=0;
		delete locks[index].ptr;
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
		mei * validator = new mei("/dev/ttyUSB0");
		printf("\nMEI Validator Initialized!\n");
		return(0);
}
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
// Command to Get the MEI Driver Version
//---------------------------------------------------------------------------------------------------

//string get_mei_driver(void)
//{
//	return get_mei_driver_version();
//}
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//  Command to Reset MEI Validator can take up to 10 seconds
//-----------------------------------------------------------------------------------------------------
void mei_reset_func(void)
{
	validator->mei_reset();
}
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// Command to get MEI Model Number (This is just a test and will become MEI Get INFO)
//-----------------------------------------------------------------------------------------------------
string mei_getmodel_func(void)
{
	string x = validator->mei_getmodel();
	return(0);
}
//-----------------------------------------------------------------------------------------------------
// Command to MEI to stack Bills
//-----------------------------------------------------------------------------------------------------
string mei_verify_bill_func(void)
{
	string x= validator->mei_verify_bill();
	return(0);
}
//------------------------------------------------------------------------------------------------------


string mei_stack(void)
{
}


string mei_inventory(void)
{
}


void MEIpoll(void)
{
}


vector<string> Split_API_Cmds(char * cmd)
{
	string cmd_string = string(cmd);
	vector<string> cmds=split(cmd_string,"-");

}


//============================================================
//					API COMMANDS
//============================================================
/*
	NOTE: all of these functions...
	RETURNS: 	TRUE on success
				FALSE on error

FOR LOCKS
Lock_Lock(ndx);
Unlock_Lock(ndx);
#define OUTTER_LOCK     0
#define INNER_LOCK      1
#define SHUTTER_LOCK    2
#define SIDECAR_LOCK    3
#define BASE_LOCK       4

*/



//declarations, must be moved UP
bool api_lock(char *cmd,int lock_index);
bool api_100(char *cmd);
bool api_101(char *cmd);
bool api_102(char *cmd);
bool api_103(char *cmd);

bool api_900(char *cmd);
bool api_901(char *cmd);
bool api_902(char *cmd);
bool api_903(char *cmd);
bool api_904(char *cmd);
string api_905(char *cmd);



/*
	universal fn used for all the lock commands
*/

bool api_lock(char *cmd,int lock_index)
{
	int res=0;
	int delay=0;
    vector<string> cmds=Split_API_Cmds(cmd);

	if (cmds[3]=="UNLOCK")
	{
		res=Unlock_Lock(lock_index);	//returns 1 on succes
	}

	if (cmds[3]=="LOCK")
	{
		res=Lock_Lock(lock_index);	//returns 1 on succes
		if (res) return TRUE;
		else	return FALSE;
	}

	if (cmds[3]=="STATUS")
	{
		res= GetIsLocked(lock_index);	// returns TRUE if locked
	}

	if (cmds[3].substr(1,5)=="DELAY")
	{
		delay=atoi(cmds[4].c_str() );
		SetLockDelay(lock_index, delay);
		res=1;
	}

	if (res) return TRUE;
	else    return FALSE;


}



// XXX = "LOCK", "UNLOCK", "STATUS", "DELAY-YY"
//100-OUTTER-DOOR-XXX
bool api_100(char *cmd)
{
    return (api_lock(cmd,OUTTER_LOCK));
}

//101-INNER-DOOR-XXX
bool api_101(char *cmd)
{
    return (api_lock(cmd,INNER_LOCK));
}


//102 and 105 come here
//102-SHUTTER-LOCK-XXX
bool api_102(char *cmd)
{
    return (api_lock(cmd,SHUTTER_LOCK));
}


//103-SIDE-CAR-LOCK-XXX
bool api_103(char *cmd)
{
    return (api_lock(cmd,SIDECAR_LOCK));
}



bool api_200(char *cmd) {}
bool api_201(char *cmd) {}
bool api_220(char *cmd) {}
bool api_221(char *cmd) {}
bool api_225(char *cmd) {}

bool api_300(char *cmd) {}
bool api_301(char *cmd) {}
bool api_302(char *cmd) {}
bool api_350(char *cmd) {}
bool api_351(char *cmd) {}
bool api_400(char *cmd) {}

bool api_500(char *cmd) {}
bool api_501(char *cmd) {}
bool api_502(char *cmd) {}
bool api_503(char *cmd) {}
bool api_504(char *cmd) {}
bool api_505(char *cmd) {}
bool api_550(char *cmd) {}


bool api_600(char *cmd) {}
bool api_601(char *cmd) {}
bool api_602(char *cmd) {}
bool api_603(char *cmd) {}
bool api_604(char *cmd) {}
bool api_650(char *cmd) {}

bool api_510(char *cmd) {}
bool api_511(char *cmd) {}
bool api_512(char *cmd) {}
bool api_513(char *cmd) {}

bool api_700(char *cmd) {}
bool api_701(char *cmd) {}
bool api_702(char *cmd) {}

bool api_800(char *cmd) {}
bool api_801(char *cmd) {}





//900-UTD-UNLOADALL
bool api_900(char *cmd)
{
	Unload_D8C(8);	// cols are 0 based
}

//901-UTD-UNLOAD-COL-X
bool api_901(char *cmd)
{
int Unload_D8C(int column);	// returns
}

//902-UTD-LOAD
bool api_902(char *cmd)
{
	Enable_Load_D8C();	// returns
}

//903-UTD-LOAD-STOP
bool api_903(char *cmd)
{
	Disable_Load_D8C();	// returns
}

//904-UTD-RESET
bool api_904(char *cmd)
{
	Reset_D8C();
	return TRUE;
}

//905-UTD-INVENTORY
// returns a string "0,0,0,0,0,0,0,0"
string api_905(char *cmd)
{
	string inv;
	// ptr returned from GetUTDInventory() points to array below
	//int utd_inventory[] =  {0, 0, 0, 0, 0, 0, 0, 0};
	int * ptr= GetUTDInventory();

	for (int n=0; n<8; n++)
	{
		inv += to_string(*ptr++);
		inv +=",";
	}

}



