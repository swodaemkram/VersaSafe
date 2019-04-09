#include <stdint.h>	// has uintmax_t
#include <string>

extern void GetDate(void);
extern int current_year;
extern int current_month;
extern int current_day;
extern int run_mode;

//extern OperationMode OpMode;	// see flow.h for definitions
extern uintmax_t EpochTime(void);
extern void MakeTimeStamp(char * timeStr);


/*
extern char flowform_IP[16];
extern int flowform_port;
extern SOCKET *flowform_client;
extern SOCKET *flowform_listener;
extern SOCKET *cmc_listener;
extern char * cmc_commands;

extern float pit_max_value;
*/

extern string err_msg;


extern char config_file[];


extern void delay(int dly);


extern int CountFileLines(char *fname, int *count, bool include_comments=FALSE);



extern char gen_buffer[255];


extern void exit_cleanup(int exit_code);
extern void GeneralSetup(void);
extern void ShowLinuxVersion(void);


extern char * TimeStamp(void);


extern void phex(string msg);
extern void phex(char * msg);


extern void ShowFreeMem(int state);



// file system oriented functions
extern bool RenameFile(char * oldname, char * newname);
extern int DeleteFile(char *filename);
extern int DeleteDirectory(char *path);
extern int file_exists(char * filename);
extern int WriteFile(char * filename, char * message);
extern bool CreateBackup(char *filename);
extern int CreateDirectory(char *path);
extern bool CopyFile(char *srcfile, char * destfile);
extern int FileDirExists(char * dirpath);
extern int MakeDir(char * dirpath);


