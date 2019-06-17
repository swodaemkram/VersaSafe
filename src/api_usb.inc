
#include "vsafe.inc"

#include "api_struc.inc"


vector<string> Split_API_Cmds(char * cmd)
{
    string cmd_string = string(cmd);
    vector<string> cmds=split(cmd_string,"-");

}


//============================================================
//                  API COMMANDS
//============================================================
/*
    NOTE: all of these functions...
    RETURNS:    TRUE on success
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


retstruc api_999(char *cmd);	//999-GET-CONFIG


bool api_lock(char *cmd,int lock_index);

retstruc api_100(char *cmd);
retstruc api_101(char *cmd);
retstruc api_102(char *cmd);
retstruc api_103(char *cmd);

// UTD
retstruc api_900(char *cmd);
retstruc api_901(char *cmd);
retstruc api_902(char *cmd);
retstruc api_903(char *cmd);
retstruc api_904(char *cmd);
retstruc api_905(char *cmd);
retstruc api_906(char *cmd);

// validator
retstruc api_920(char *cmd);
retstruc api_921(char *cmd);
retstruc api_922(char *cmd);
retstruc api_923(char *cmd);
retstruc api_924(char *cmd);
//retstruc api_925(char *cmd);

/*
string Validator_verify(void) {};
string Validator_stack(int which) {};
string Validator_model(int which) {};
string Validator_serial(int which) {};
string Validator_info(int which) {};
void Validator_reset(void) {};
*/


#define ENABLECALLS


int FindWhich(char *cmd)
{
	int which;
    if (strstr(cmd,"LEFT") != NULL) which=VALIDATOR_LEFT;
    if (strstr(cmd,"RIGHT") != NULL) which=VALIDATOR_RIGHT;
    if (strstr(cmd,"UTD") != NULL) which=VALIDATOR_UCD;
	return which;
}


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
        res=Unlock_Lock(lock_index);    //returns 1 on succes
    }

    if (cmds[3]=="LOCK")
    {
        res=Lock_Lock(lock_index);  //returns 1 on succes
        if (res) return TRUE;
        else    return FALSE;
    }

    if (cmds[3]=="STATUS")
    {
        res= GetIsLocked(lock_index);   // returns TRUE if locked
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



/*
	return the config.xml file
*/
retstruc api_999(char *cmd)
{
    retstruc retstat;
//	retstat.result(2000,0);
	retstat.result = SerializeConfig();	// in vsafe.cpp

	printf("%s",retstat.result.c_str());

	return retstat;
}



// XXX = "LOCK", "UNLOCK", "STATUS", "DELAY-YY"
//100-OUTTER-DOOR-XXX
retstruc api_100(char *cmd)
{
	retstruc retstat;
    retstat.status= api_lock(cmd,OUTTER_LOCK);
}

//101-INNER-DOOR-XXX
retstruc api_101(char *cmd)
{
	retstruc retstat;
    retstat.status= api_lock(cmd,INNER_LOCK);
}


//102 and 105 come here
//102-SHUTTER-LOCK-XXX
retstruc api_102(char *cmd)
{
    retstruc retstat;
    retstat.status= api_lock(cmd,SHUTTER_LOCK);
}


//103-SIDE-CAR-LOCK-XXX
retstruc api_103(char *cmd)
{
    retstruc retstat;
    retstat.status =api_lock(cmd,SIDECAR_LOCK);
}


//200-START-DEPOSIT-CASH
retstruc api_200(char *cmd) {}

//201-STOP-DEPOSIT-CASH
retstruc api_201(char *cmd) {}

//220-START-MANUAL-DEPOSIT
retstruc api_220(char *cmd) {}

//221-STOP-MANUAL-DEPOSIT
retstruc api_221(char *cmd) {}

//225-RESET-CASSETTES
retstruc api_225(char *cmd)
{
    retstruc retstat;
#ifdef ENABLECALLS
	Validator_reset();
#endif
	return retstat;
}








//300-USER-LOGIN-user-pw
retstruc api_300(char *cmd)
{
    string cmd_string = string(cmd);
    vector<string> cmds=split(cmd_string,"-");
	retstruc retstat;

	// username = cmds[3]
	// pw = cmds[4]

	/*
	    validate the user/pw in the DBF
	    returns TRUE if valid, else FALSE
	*/
	retstat.status=ValidateUser( (char*)cmds[3].c_str() , (char*)cmds[4].c_str() );
	return retstat;	// true if valid user, else false
}




//301-USER-LOGOUT
retstruc api_301(char *cmd)
{
	retstruc retstat;
	logged_in=FALSE;
	retstat.status=TRUE;
	return retstat;
}


//302-USER-CHANGE-PW-oldpw-newpw
retstruc api_302(char *cmd) {}

//350-SHIFT-START
retstruc api_350(char *cmd) {}

//351-SHIFT-STOP
retstruc api_351(char *cmd) {}

//400-VEND-CHANGE
retstruc api_400(char *cmd) {}

//500-REPORT-RECEIPTS
retstruc api_500(char *cmd) {}

//501-DAY-TOTALS
retstruc api_501(char *cmd) {}

//502-USER-TRANS-HIST
retstruc api_502(char *cmd) {}

//503-USER-TOTALS
retstruc api_503(char *cmd) {}

//504-SHIFT-TOTALS
retstruc api_504(char *cmd) {}

//505-VEND-INVENTORY
retstruc api_505(char *cmd) {}

//550-CHANGE-SOUND
retstruc api_550(char *cmd) {}

//600-PERFORM-EOD
retstruc api_600(char *cmd) {}

//601-VEND-UNLOAD
retstruc api_601(char *cmd) {}

//602-VEND-LOAD
retstruc api_602(char *cmd) {}

//603-CONTENT-REMOVAL
retstruc api_603(char *cmd) {}

//604-VEND-DEFINE
retstruc api_604(char *cmd) {}

//650-SET-LOCK-DELAY-X
retstruc api_650(char *cmd) {}

//510-ADD-SHIFT-00:00-00:00
retstruc api_510(char *cmd) {}

//511-DEL-SHIFT-00:00-00:00
retstruc api_511(char *cmd) {}

//512-START-SHIFT-00:00-00:00
retstruc api_512(char *cmd) {}

//513-END-SHIFT-00:00-00:00
retstruc api_513(char *cmd) {}

//700-BACKUP-DBF
retstruc api_700(char *cmd) {}

//701-FLASH-VALIDATOR
retstruc api_701(char *cmd) {}

//702-FLASH-UTD
retstruc api_702(char *cmd) {}

//800-VALIDATOR-VALIDATE
retstruc api_800(char *cmd) {}

//801-VALIDATOR-ACCEPT
retstruc api_801(char *cmd) {}



//900-UTD-UNLOADALL
retstruc api_900(char *cmd)
{
    Unload_D8C(8);  // cols are 0 based
}

//901-UTD-UNLOAD-COL-X
retstruc api_901(char *cmd)
{
int Unload_D8C(int column); // returns
}

//902-UTD-LOAD
retstruc api_902(char *cmd)
{
    Enable_Load_D8C();  // returns
}

//906-UTD-UNLOAD-COLS-1,2,3,4,5
retstruc api_906(char *cmd)
{

    string columns;
    // parse out the commands to cmds[]
    vector<string> cmds=Split_API_Cmds(cmd);
    // parse out the columns to cls[]
    vector<string> cols=split( cmds[4],"," );


    // now build a space delimited string for the driver
    for (int n=0; n < cols.size(); n++)
        columns +=cols[n] + " ";

    Unload_D8C(columns);
}


//903-UTD-LOAD-STOP
retstruc api_903(char *cmd)
{
    Disable_Load_D8C(); // returns
}

//904-UTD-RESET
retstruc api_904(char *cmd)
{
	retstruc retstat;
	retstat.status=true;
    Reset_D8C();
    retstat.status=TRUE;
	return retstat;
}

//905-UTD-INVENTORY
// returns a string "0,0,0,0,0,0,0,0"
retstruc api_905(char *cmd)
{
	retstruc retstat;
    // ptr returned from GetUTDInventory() points to array below
    //int utd_inventory[] =  {0, 0, 0, 0, 0, 0, 0, 0};
    int * ptr= GetUTDInventory();

    for (int n=0; n<8; n++)
    {
        retstat.result += to_string(*ptr++);
        retstat.result +=",";
    }

    return retstat;
}




//VALIDATOR (MEI/JCM)
//920-VALIDATOR-VERIFY
retstruc api_920(char *cmd)
{
    retstruc retstat;
#ifdef ENABLECALLS
    retstat.result=Validator_verify();
#endif
    return retstat;
}


//921-VALIDATOR-STACK-(LEFT|RIGHT|UCD)
retstruc api_921(char *cmd)
{
    retstruc retstat;

	int which = FindWhich(cmd);

#ifdef ENABLECALLS
    retstat.result=Validator_stack(which);
#endif
    return retstat;
}

//922-VALIDATOR-MODEL-(LEFT|RIGHT|UCD)
retstruc api_922(char *cmd)
{
    retstruc retstat;

    int which = FindWhich(cmd);
#ifdef ENABLECALLS
    retstat.result=Validator_model(which);
#endif
    return retstat;
}

//923-VALIDATOR-SERIAL-(LEFT|RIGHT|UCD)
retstruc api_923(char *cmd)
{
    retstruc retstat;
    int which = FindWhich(cmd);
#ifdef ENABLECALLS
    retstat.result=Validator_serial(which);
#endif
    return retstat;
}

//924-VALIDATOR-INFO-(LEFT|RIGHT|UCD)
retstruc api_924(char *cmd)
{
    retstruc retstat;
    int which = FindWhich(cmd);
#ifdef ENABLECALLS
    retstat.result=Validator_info(which);
#endif
    return retstat;
}


//925-VALIDATOR-RESET-(LEFT|RIGHT)
retstruc api_925(char *cmd)
{
    retstruc retstat;
    int which = FindWhich(cmd);
#ifdef ENABLECALLS
    retstat.result=Validator_reset();
#endif
    return retstat;
}






