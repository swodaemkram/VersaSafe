// external declarations for usb_gateway.cpp

#include <string>


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
extern void mei_verify_bill_func(void);
extern string mei_getmodel_func(void);
extern void mei_stack(void);
extern void MEIpoll(void);
extern int mei_send_command(string);
extern string mei_get_response(void);
extern string get_mei_serialNumber(void);
extern void mei_shutdown(void);

// api commands
extern bool api_100(char *cmd);
extern bool api_101(char *cmd);
extern bool api_102(char *cmd);
extern bool api_103(char *cmd);
//extern bool api_105(char *cmd);

extern bool api_200(char *cmd);
extern bool api_201(char *cmd);
extern bool api_220(char *cmd);
extern bool api_221(char *cmd);
extern bool api_225(char *cmd);

extern bool api_300(char *cmd);
extern bool api_301(char *cmd);
extern bool api_302(char *cmd);
extern bool api_350(char *cmd);
extern bool api_351(char *cmd);
extern bool api_400(char *cmd);

extern bool api_500(char *cmd);
extern bool api_501(char *cmd);
extern bool api_502(char *cmd);
extern bool api_503(char *cmd);
extern bool api_504(char *cmd);
extern bool api_505(char *cmd);
extern bool api_550(char *cmd);

extern bool api_600(char *cmd);
extern bool api_601(char *cmd);
extern bool api_602(char *cmd);
extern bool api_603(char *cmd);
extern bool api_604(char *cmd);
extern bool api_650(char *cmd);

extern bool api_510(char *cmd);
extern bool api_511(char *cmd);
extern bool api_512(char *cmd);
extern bool api_513(char *cmd);

extern bool api_700(char *cmd);
extern bool api_701(char *cmd);
extern bool api_702(char *cmd);

extern bool api_800(char *cmd);
extern bool api_801(char *cmd);

extern bool api_900(char *cmd);
extern bool api_901(char *cmd);
extern bool api_902(char *cmd);
extern bool api_903(char *cmd);
extern bool api_904(char *cmd);
extern string api_905(char *cmd);
extern bool api_906(char *cmd);

