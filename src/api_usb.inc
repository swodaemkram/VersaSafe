
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



//declarations, must be moved UP
bool api_lock(char *cmd,int lock_index);
bool api_100(char *cmd);
bool api_101(char *cmd);
bool api_102(char *cmd);
bool api_103(char *cmd);

// UTD
bool api_900(char *cmd);
bool api_901(char *cmd);
bool api_902(char *cmd);
bool api_903(char *cmd);
bool api_904(char *cmd);
string api_905(char *cmd);
bool api_906(char *cmd);

// validator
string api_920(char *cmd);
string api_921(char *cmd);
string api_922(char *cmd);
string api_923(char *cmd);
string api_924(char *cmd);




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


//300-USER-LOGIN-user-pw
bool api_300(char *cmd)
{
    string cmd_string = string(cmd);
    vector<string> cmds=split(cmd_string,"-");

	// username = cmds[3]
	// pw = cmds[4]

	/*
	    validate the user/pw in the DBF
	    returns TRUE if valid, else FALSE
	*/
	bool uservalid=ValidateUser(cmds[3].c_str() ,cmds[4].c_str() );

}

//301-USER-LOGOUT
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
    Unload_D8C(8);  // cols are 0 based
}

//901-UTD-UNLOAD-COL-X
bool api_901(char *cmd)
{
int Unload_D8C(int column); // returns
}

//902-UTD-LOAD
bool api_902(char *cmd)
{
    Enable_Load_D8C();  // returns
}

//906-UTD-UNLOAD-COLS-1,2,3,4,5
bool api_906(char *cmd)
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
bool api_903(char *cmd)
{
    Disable_Load_D8C(); // returns
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

    return inv;
}



//VALIDATOR (MEI/JCM)
//920-VALIDATOR-VERIFY
string api_920(char *cmd)
{
}


//921-VALIDATOR-STACK
string api_921(char *cmd)
{
}

//922-VALIDATOR-MODEL
string api_922(char *cmd)
{
}

//923-VALIDATOR-SERIAL
string api_923(char *cmd)
{
}

//924-VALIDATOR-INVENTORY
string api_924(char *cmd)
{
}

//925-VALIDATOR-GET-RESULT
string api_925(char *cmd)
{
}


