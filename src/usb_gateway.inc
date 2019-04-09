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
