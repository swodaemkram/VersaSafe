// external declarations for usb_gateway.cpp

#ifndef _USBGATEWAYINC

#define _USBGATEWAYINC

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
extern void GetLockStatus(int index);
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

//extern int init_mei(void);
extern void mei_reset_func(void);
extern string mei_verify_bill_func(void);
extern string mei_getmodel_func(void);
//extern string get_mei_driver(void);
//extern int mei_connect(void);
//extern string mei_getmodel(void);
extern string mei_stack(void);
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
extern int init_validators(void);

extern string Validator_verify(void);
extern string Validator_stack(int which);
extern string Validator_model(int which);
extern string Validator_serial(int which);
extern string Validator_info(int which);
extern void Validator_reset(int which);
extern string Validator_idle(int which);
extern string Validator_get_results(int which);
extern void Validator_done(int which);


#endif

