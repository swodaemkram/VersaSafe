
#include "api_struc.inc"		// retstruc for function returns


/*
	externals for inclusion in api.cpp
	the functions in in usb_gateway.cpp and this file is included there

*/

extern bool api_lock(char *cmd,int lock_index);

// XXX = "LOCK", "UNLOCK", "STATUS", "DELAY-YY"
extern retstruc api_100(char *cmd);		//100-OUTTER-DOOR-XXX
extern retstruc api_101(char *cmd);		//101-INNER-DOOR-XXX
extern retstruc api_102(char *cmd);		//102-SHUTTER-LOCK-XXX
extern retstruc api_103(char *cmd);		//103-SIDE-CAR-LOCK-XXX


extern retstruc api_200(char *cmd);		//200-START-DEPOSIT-CASH
extern retstruc api_201(char *cmd);		//201-STOP-DEPOSIT-CASH
extern retstruc api_220(char *cmd);		//220-START-MANUAL-DEPOSIT
extern retstruc api_221(char *cmd);		//221-STOP-MANUAL-DEPOSIT
extern retstruc api_225(char *cmd);		//225-RESET-CASSETTES

extern retstruc api_300(char *cmd);		//300-USER-LOGIN-user-pw
extern retstruc api_301(char *cmd);		//301-USER-LOGOUT
extern retstruc api_302(char *cmd);		//302-USER-CHANGE-PW-oldpw-newpw
extern retstruc api_350(char *cmd);		//350-SHIFT-START
extern retstruc api_351(char *cmd);		//351-SHIFT-STOP

extern retstruc api_400(char *cmd);		//400-VEND-CHANGE

extern retstruc api_500(char *cmd);		//500-REPORT-RECEIPTS
extern retstruc api_501(char *cmd);		//501-DAY-TOTALS
extern retstruc api_502(char *cmd);		//502-USER-TRANS-HIST
extern retstruc api_503(char *cmd);		//503-USER-TOTALS
extern retstruc api_504(char *cmd);		//504-SHIFT-TOTALS
extern retstruc api_505(char *cmd);		//505-VEND-INVENTORY
extern retstruc api_550(char *cmd);		//550-CHANGE-SOUND

extern retstruc api_600(char *cmd);		//600-PERFORM-EOD
extern retstruc api_601(char *cmd);		//601-VEND-UNLOAD
extern retstruc api_602(char *cmd);		//602-VEND-LOAD
extern retstruc api_603(char *cmd);		//603-CONTENT-REMOVAL
extern retstruc api_604(char *cmd);		//604-VEND-DEFINE
extern retstruc api_650(char *cmd);		//650-SET-LOCK-DELAY-X

extern retstruc api_510(char *cmd);		//510-ADD-SHIFT-00:00-00:00
extern retstruc api_511(char *cmd);		//511-DEL-SHIFT-00:00-00:00
extern retstruc api_512(char *cmd);		//512-START-SHIFT-00:00-00:00
extern retstruc api_513(char *cmd);		//513-END-SHIFT-00:00-00:00

extern retstruc api_700(char *cmd);		//700-BACKUP-DBF
extern retstruc api_701(char *cmd);		//701-FLASH-VALIDATOR
extern retstruc api_702(char *cmd);		//702-FLASH-UTD




// UTD
extern retstruc api_900(char *cmd);		//900-UTD-UNLOADALL
extern retstruc api_901(char *cmd);		//901-UTD-UNLOAD-COL-X
extern retstruc api_902(char *cmd);		//902-UTD-LOAD
extern retstruc api_903(char *cmd);		//903-UTD-LOAD-STOP
extern retstruc api_904(char *cmd);		//904-UTD-RESET
extern retstruc api_905(char *cmd);		//905-UTD-INVENTORY
extern retstruc api_906(char *cmd);		//906-UTD-UNLOAD-COLS-1,2,3,4,5

// validator
extern retstruc api_920(char *cmd);		//920-VALIDATOR-VERIFY
extern retstruc api_921(char *cmd);		//921-VALIDATOR-STACK
extern retstruc api_922(char *cmd);		//922-VALIDATOR-MODEL
extern retstruc api_923(char *cmd);		//923-VALIDATOR-SERIAL
extern retstruc api_924(char *cmd);		//924-VALIDATOR-INFO
extern retstruc api_925(char *cmd);		//925-VALIDATOR-RESET-(LEFT|RIGHT)
extern retstruc api_926(char *cmd);		//926-VALIDATOR-IDLE-(LEFT|RIGHT)
extern retstruc api_927(char *cmd);		//927-VALIDATOR-GET-RESULTS(LEFT|RIGHT)



extern retstruc api_999(char *cmd);		//999-GET-CONFIG
