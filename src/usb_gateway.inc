// external declarations for usb_gateway.cpp

#include <string>

#include "api_struc.inc"

extern void USB_init(void);
extern void USB_shutdown(void);


// LOCK SPECIFIC FUNCTIONS
extern int Init_USBX(int index);
extern void Kill_USBX(int index);
extern int Lock_Lock(int index) ;
extern int Unlock_Lock(int index);
extern int SetLockSerial(int index, string ser);
extern string GetLockSerial(int index);
extern void SetLockDelay(int index, int delay);
extern string GetLockFirmware(int index);
extern string GetLockVolts(int index);
extern string GetLockStatus(int index);
extern string GetLockDriverVersion(void);
extern void PrintLockStats(int index);
extern string GetLockType(int index);
extern int GetLockDelay(int index);
extern int DownloadLockFirmware(int index);
extern bool GetIsLocked(int index);
extern void Enable_Lock(int index);


// D8C specific functions
extern int Init_D8C(void);
extern void Reset_D8C(void);
extern int Unload_D8C(string cols);
extern int Unload_D8C(int column);
extern string Get_d8_driver(void);
extern void Reset_D8C(void);
extern int Disable_Load_D8C(void);
extern int Enable_Load_D8C(void);
extern int Unload_D8C(string cols);    // cols are zero based
extern int Unload_D8C(int column);
extern string Get_d8_driver(void);
extern int * GetUTDInventory(void);
extern bool D8C_detected(void);


// MEI specific functions

extern int init_mei(void);
extern void mei_reset_func(void);
extern string mei_verify_bill_func(void);
extern string mei_getmodel_func(void);
//extern string get_mei_driver(void);
//extern int mei_connect(void);
//extern string mei_getmodel(void);
extern string mei_stack(void);
extern string mei_inventory(void);
//extern string mei_get_info(void);
//extern void mei_enable(void);
//extern void mei_dissable(void);
//extern void mei_accept(void);
//extern void mei_enable_bookmark(void);
//extern void mei_enable_in(int meienableincount);
//extern void mei_accept_in(string meibillstoaccept);
//extern void mei_dissable_time(int meidissabletimeinsec);
//extern int * mei_get_inventory(void);
//extern bool mei_detected(void);
//extern string mei_status(void);
extern void MEIpoll(void);


extern retstruc api_999(char *cmd);


// api commands
extern retstruc api_100(char *cmd);
extern retstruc api_101(char *cmd);
extern retstruc api_102(char *cmd);
extern retstruc api_103(char *cmd);
//extern retstruc api_105(char *cmd);

extern retstruc api_200(char *cmd);
extern retstruc api_201(char *cmd);
extern retstruc api_220(char *cmd);
extern retstruc api_221(char *cmd);
extern retstruc api_225(char *cmd);

extern retstruc api_300(char *cmd);
extern retstruc api_301(char *cmd);
extern retstruc api_302(char *cmd);
extern retstruc api_350(char *cmd);
extern retstruc api_351(char *cmd);
extern retstruc api_400(char *cmd);

extern retstruc api_500(char *cmd);
extern retstruc api_501(char *cmd);
extern retstruc api_502(char *cmd);
extern retstruc api_503(char *cmd);
extern retstruc api_504(char *cmd);
extern retstruc api_505(char *cmd);
extern retstruc api_550(char *cmd);

extern retstruc api_600(char *cmd);
extern retstruc api_601(char *cmd);
extern retstruc api_602(char *cmd);
extern retstruc api_603(char *cmd);
extern retstruc api_604(char *cmd);
extern retstruc api_650(char *cmd);

extern retstruc api_510(char *cmd);
extern retstruc api_511(char *cmd);
extern retstruc api_512(char *cmd);
extern retstruc api_513(char *cmd);

extern retstruc api_700(char *cmd);
extern retstruc api_701(char *cmd);
extern retstruc api_702(char *cmd);

extern retstruc api_800(char *cmd);
extern retstruc api_801(char *cmd);

extern retstruc api_900(char *cmd);
extern retstruc api_901(char *cmd);
extern retstruc api_902(char *cmd);
extern retstruc api_903(char *cmd);
extern retstruc api_904(char *cmd);
extern retstruc api_905(char *cmd);

extern retstruc api_906(char *cmd);

//VALIDATORS
extern retstruc api_920(char *cmd);
extern retstruc api_921(char *cmd);
extern retstruc api_922(char *cmd);
extern retstruc api_923(char *cmd);
extern retstruc api_924(char *cmd);
extern retstruc api_925(char *cmd);

