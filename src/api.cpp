/*
    Program: VSAFE
    Module: api.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 4-8-2019
    Updated:
    Language: C,C++ (this module is C++ compilable and linkable with C libs)


	API module

	1. read API.TXT file containing all valid API commands
	2. create a listener socket on cfg.api_port
	3. make connections and receive commands from client
	4. echo cmds back to client as ACK (or a result)
	5. process any valid commands and execute via CommandDispatcher()



*/

#include <sstream>
#include <cstdio>
#include <cstdlib>




#include "hdr/config.h"
#include "usb_gateway.inc"
#include "hdr/usb_gateway.h"
// logs.cpp
#include "logs.inc"
#include "trim.inc"
#include "hdr/fire.h"

//#include "api_struc.inc"
#include "api_usb_ext.inc"

void SetupAPI(void);
bool CreateAPI(void);
char * ListenAPI(void);
retstruc CommandDispatcher(char * cmd);
int Read_API_File(void);

extern int GetAPIport(void);	// in vsafe.cpp


SOCKET * api = new SOCKET(TCP_CONNECTION); // instantiate our API socket

int api_port;


#define MAX_API 200
struct myapi
{
	char cmd[80];
};

myapi api_cmds[MAX_API];

bool api_avail=FALSE;


/*
	1. read the API.TXT file
	2. create a listener socket
	3. start listening

*/
void SetupAPI(void)
{
	int res;
	api_avail=TRUE;
	res=Read_API_File();
	if (res != OK)	api_avail=FALSE;

	if (!api_avail) return;	// disable API if no CMD file
	res=CreateAPI();
	if (!res) api_avail=FALSE;

	ListenAPI();
}




/*

    create a listener socket

*/

bool CreateAPI(void)
{
	int res;
	api_port = GetAPIport();

printf("API PORT: %d\n",api_port);

	// RETURNS 0 on success
	// 704 unable to create receive buffer
	// 705 error creating general purpose buffer
	res=api->Server(api_port);

	if (res !=0 )
	{
		// res has the error code
		return FALSE;
	}
	else
	{
		printf("API socket created\n");
		return TRUE;
	}
}



/*
	if data is available on our socket, then get it and return it

	RETURNS: char* to received string or NULL
*/
char * ListenAPI(void)
{
	char my_buffer[2000];
	bool ret;
	bool status;
	int bytecount;
    char *ptr;
	int socket;
	retstruc retstat;

	retstat.result.reserve(2000);

    if (!api_avail) return NULL;  // API disabled if no CMD file


	// call ReceiveMessage() in socket_class.cpp
    ptr=api->ReceiveMessage(&bytecount,&socket);         // see if any data recieved from other computer
    if (ptr == NULL)
    {
        // no data
		return NULL;
    }
    else
    {
printf("RECVD FROM CLIENT::%s\n",ptr);
		retstat=CommandDispatcher(ptr);		// execute the command

printf("RETSTAT::  cmd:%d  status:%d\n",retstat.cmd,retstat.status);

		switch(retstat.cmd)
		{
		case 300:		// user login
			if (retstat.status)
				sprintf(my_buffer,"%s-OK",ptr);
			else
				sprintf(my_buffer,"%s-ERROR",ptr);
			ptr=my_buffer;
			break;
		case 999:		// return serialized config.xml file

			sprintf(my_buffer,"%s",(retstat.result.substr(0,100)).c_str() );
printf("GRC:%s\n",my_buffer);
sprintf(my_buffer,"%s",retstat.result.c_str() );
//sprintf(my_buffer,"we are testing here");
			ptr=my_buffer;
printf("returning 999\n");
		}

		ret=api->SendMessage(ptr,socket);	// echo what we received (or a result)
		if (!ret)
		{
	        sprintf(my_buffer,"ListenAPI::ERROR writing to socket");
	        WriteSystemLog(my_buffer);
		}

//        ptr points to our char[] data
//        bytecount has number of received bytes
		return ptr;
    }


}



/*
	read the api.txt file, containing all API commands
*/

int Read_API_File(void)
{
    #define MAX_LINE_LEN 255
    FILE *apifile=NULL;
    char linebuf[MAX_LINE_LEN];
	char fname[20]="api.txt";
	int msg_count=0;

    apifile=fopen(fname,"r");
    if (!apifile)
    {
        printf("Unable to read API file: %s\n",fname);
        return ERROR;
    }

do
{

    if (fgets(linebuf, MAX_LINE_LEN, apifile) == NULL) break; // read a line
    if (feof(apifile)) break;
    if (ferror(apifile))
    {
        fclose(apifile);
        return ERROR;
    }

    chomp(linebuf); // strip CRLF and make ASCIZ
    trim(linebuf);

    if (linebuf[0] == '#') continue;        // skip comments
    if (strlen(linebuf) ==0) continue;      // skip if blank line

    // SAMPLE LINE: "
    strcpy(api_cmds[msg_count].cmd,&linebuf[0]);


    msg_count++;

//	printf("API:%d:%s\n",msg_count,linebuf);

} while (! feof(apifile));

    fclose(apifile);
    printf("API file processed successfully\r\n");

	return OK;

}



/*
	cmd has the command string actually sent by the client
	call the apporpriate function for that command

	EXAMPLE CMD: "105-UNLOCK-VALIDATOR-GATE"


*/

retstruc CommandDispatcher(char * cmd)
{
	string ret;
//	bool status;
	string invt;
	string cmd_string = string(cmd);
	vector<string> cmds=split(cmd_string,"-");
	retstruc retstat;


	// first element of the cmd is the cmd number, use that for dispatching
	int cmd_num = atoi(cmds[0].c_str() );
/*
	NOTE: at first glance, it might seem wise to set retstat.cmd = cmd_num right here,
		however, retstat is a pointer to a struct and is pointing to a specific struct by the given api_xxx() function
		is called, therefore, retstat.cmd would be overwritten.
*/


	switch(cmd_num)
	{
	case 999:	//999-GET-CONFIG
	    retstat.result.reserve(2000);
		retstat=api_999(cmd);
		retstat.cmd=999;
		break;
				// XXX = "LOCK", "UNLOCK", "STATUS", "DELAY-YY"
	case 100:	//100-OUTTER-DOOR-XXX
		api_100(cmd);
		break;
	case 101:	//101-INNER-DOOR-XXX
		api_101(cmd);
		break;


    case 105:   //105-UNLOCK-BILLREADER-GATE
		api_102( (char *)"102-SHUTTER-LOCK-UNLOCK");
		break;
	case 102:	//102-SHUTTER-LOCK-XXX
        api_102(cmd);
		break;
	case 103:	//103-SIDE-CAR-LOCK-XXX
        api_103(cmd);
		break;

	case 200:	//200-START-DEPOSIT-CASH
        api_200(cmd);
		break;
	case 201:	//201-STOP-DEPOSIT-CASH
        api_201(cmd);
		break;
	case 220:	//220-START-MANUAL-DEPOSIT
        api_220(cmd);
		break;
	case 221:	//221-STOP-MANUAL-DEPOSIT
        api_221(cmd);
		break;
	case 225:	//225-RESET-CASSETTES
        api_225(cmd);
		break;

	case 300:	//	300-USER-LOGIN-user-pw
		retstat=api_300(cmd);
		retstat.cmd=300;
		break;
	case 301:	//301-USER-LOGOUT
        retstat=api_301(cmd);
		retstat.cmd=301;
		break;
	case 302:	//302-USER-CHANGE-PW-oldpw-newpw
        api_302(cmd);
		break;


	case 350:	//350-SHIFT-START
        api_350(cmd);
		break;
	case 351:	//351-SHIFT-STOP
        api_351(cmd);
		break;

	case 400:	//400-VEND-CHANGE
        api_400(cmd);
		break;

		// REPORTS

	case 500:	//500-REPORT-RECEIPTS
       api_500(cmd);
		break;
	case 501:	//501-DAY-TOTALS
       api_501(cmd);
		break;
	case 502:	//502-USER-TRANS-HIST
       api_502(cmd);
		break;
	case 503:	//503-USER-TOTALS
       api_503(cmd);
		break;
	case 504:	//504-SHIFT-TOTALS
       api_504(cmd);
		break;
	case 505:	//505-VEND-INVENTORY
       api_505(cmd);
		break;

	//SETTINGS
	case 550:	//550-CHANGE-SOUND
       api_550(cmd);
		break;

	//#ADMIN

	case 600:	//600-PERFORM-EOD
       api_600(cmd);
		break;
	case 601:	//601-VEND-UNLOAD
       api_601(cmd);
		break;
	case 602:	//602-VEND-LOAD
       api_602(cmd);
		break;
	case 603:	//603-CONTENT-REMOVAL
       api_603(cmd);
		break;
	case 604:	//604-VEND-DEFINE
       api_604(cmd);
		break;

	case 650:	//650-SET-LOCK-DELAY-X
       api_650(cmd);
		break;

	case 510:	//510-ADD-SHIFT-00:00-00:00
       api_510(cmd);
		break;
	case 511:	//511-DEL-SHIFT-00:00-00:00
       api_511(cmd);
		break;
	case 512:	//512-START-SHIFT-00:00-00:00
       api_512(cmd);
		break;
	case 513:	//513-END-SHIFT-00:00-00:00
       api_513(cmd);
		break;

	//#MAINT

	case 700:	//600-BACKUP-DBF
       api_700(cmd);
		break;
	case 701:	//701-FLASH-VALIDATOR
       api_701(cmd);
		break;
	case 702:	//702-FLASH-UTD
       api_702(cmd);
		break;
/*
	case 800:	//800-VALIDATOR-VALIDATE
       api_800(cmd);
		break;
	case 801:	//801-VALIDATOR-ACCEPT
       api_801(cmd);
		break;
*/

	case 900:	//900-UTD-UNLOADALL
       api_900(cmd);
		retstat.cmd=900;
		break;

	// cols are zero based (0-7), 8 =unload all
	case 901:	//901-UTD-UNLOAD-COL-X
       api_901(cmd);
        retstat.cmd=901;
		break;
	case 902:	//902-UTD-LOAD
       api_902(cmd);
        retstat.cmd=902;
		break;

	case 903:	//903-UTD-LOAD-STOP
		api_903(cmd);
        retstat.cmd=903;
		break;
	case 904:	//904-UTD-RESET
		api_904(cmd);
        retstat.cmd=904;
		break;
	case 905:	//905-UTD-INVENTORY
		retstat=api_905(cmd);	// returns inventory as a string , eg "1,4,5,2,3,6,0,0"
        retstat.cmd=905;
		break;
	case 906:				//906-UTD-UNLOAD-COLS-1,2,3,4,5
		api_906(cmd);
        retstat.cmd=906;
		break;
	case 907:
		api_907(cmd);	//907-UTD-INFO
		retstat.cmd=907;
		break;

	case 920:		//920-VALIDATOR-VERIFY
		retstat = api_920(cmd);
        retstat.cmd=920;
		break;
	case 921:		//921-VALIDATOR-STACK
		retstat=api_921(cmd);
        retstat.cmd=921;
		break;
	case 922:		//922-VALIDATOR-MODEL
		retstat=api_922(cmd);
        retstat.cmd=922;
		break;
	case 923:		//923-VALIDATOR-SERIAL
		retstat=api_923(cmd);
        retstat.cmd=923;
		break;
	case 924:		//924-VALIDATOR-INFO
		retstat=api_924(cmd);
		retstat.cmd=924;
		break;
	case 925:		//925-VALIDATOR-RESET-(LEFT|RIGHT)
		retstat=api_925(cmd);
        retstat.cmd=925;
		break;
    case 926:       //926-VALIDATOR-IDLE-(LEFT|RIGHT)
        retstat=api_926(cmd);
        retstat.cmd=926;
        break;

	case 927:		//927-VALIDATOR-GET-RESULTS-(LEFT|RIGHT)
		retstat=api_927(cmd);
		retstat.cmd=927;
		break;

	case 998:		//998-VALIDATOR_DONE-(LEFT|RIGHT)
		retstat=api_998(cmd);
		retstat.cmd=998;
		break;
	}


	return retstat;

}





