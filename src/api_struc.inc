
#ifndef _RETSTRUC

#define _RETSTRUC

struct rets
{
	int cmd;			// cmd from caller
    bool status;		// bool return status
	int int_result;		// int return
    string result;		// string return
};

typedef rets retstruc;

//retstruc retstat;

#endif

