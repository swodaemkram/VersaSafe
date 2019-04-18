/*
	Module: common.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 12-7-2017
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2017 FireKing Security Group

    Version: 1.0


	this module is common between flowform.cpp and production.cpp


METRICS:

	Ubuntu 17.04

*/

#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <stdint.h>	// has uintmax_t

#include "hdr/config.h"
#include "hdr/fire.h"
#include "locale/locale.h"
#include "hdr/socket_class.h"
#include "usb_gateway.inc"


// PUBLIC FUNCTIONS
void exit_cleanup(int exit_code);
int WriteFile(char * filename, char * message);
char * TimeStamp(void);
uintmax_t EpochTime(void);	// return seconds since EPOCH
//int CountFileLines(char *fname, int *count);
void MakeTimeStamp(char * timeStr);

void GeneralSetup(void);

void delay(int dly);
void ShowFreeMem(int state);
void GetDate(void);
void ShowLinuxVersion(void);
int CreateDirectory(char *path);


// file system oriented functions
bool RenameFile(char * oldname, char * newname);
int DeleteFile(char *filename);
int DeleteDirectory(char *path);
int file_exists(char * filename);
bool CreateBackup(char *filename);
bool CopyFile(char *srcfile, char * destfile);
int FileDirExists(char * dirpath);
int MakeDir(char * dirpath);





char gen_buffer[255];	// general purpose buffer, use and lose

char timeStr[50]="";

char config_file[]=	"xml/config.xml";
char config_backup[] = "../backup/flowformrc.bak";
char tstamp[50];

// the following are set by GetDate()
int current_year;
int current_month;
int current_day;

bool fhs_ready_bit;
int inches_mm;	// 0=inches, 1=mm


/*
SOCKET *flowform_client;
SOCKET *flowform_listener;
SOCKET *cmc_listener;
char * cmc_commands=NULL;
*/


// this is a stub, the actual array must be created dynamically from the /lang/ folder
//extern const char * languages[];
//extern int curLanguage;			// index into the above array

// in locale.cpp
extern country * countries;
extern int SetLang(const char *filename);




// EXTERNAL DECLARATIONS




// trim module functions
#include "trim.inc"

// ./segfault.cpp
extern void TrapSegFault(void);


// ip_module.cpp
#include "ip_module.inc"





// help.cpp
extern void CloseHelp(void);





#define CONFIG_VERSION 1.0
float config_version;
string err_msg;




//============================================================================
// the following are as read from the config file




int run_mode=0;	// 0 = normal FlowForm
				// 1 = thin client (repeater monitor)

char flowform_IP[16];
int flowform_port;	// port where thin clients connect

int pdo_port=43211;



//============================================================================





void GeneralSetup(void)
{
	TrapSegFault();
	SetLang("english");		// in ..//locale/locale.cpp
//	USB_init();

	#if PGM_FLAVOR==PRODUCTION_PGM
		ScannerInit();
	#endif

}

/*
	rename/move a file

	RETURNS: TRUE on success
			FALSE on failure
*/


bool RenameFile(char * oldname, char * newname)
{
	if ( rename( oldname, newname) == 0)
		return TRUE;
	else
		return FALSE;
}



/*
	copy the filename to filename.bak
	if filename.bak already exists, delete it
	same as: cp file.ext file.ext.bak in linux

	RETURNS:	TRUE on success
				FALSE on failure
*/

bool CreateBackup(char *filename)
{
	char destfile[500];

	// if the file doesn't exist, pretend like we copied it OK
	if ( !file_exists(filename)) return TRUE;

	try
	{
		strcpy(destfile,filename);
		strcat(destfile,".bak");

		// if there's an existing backup, then kill it
		DeleteFile(destfile);

		// now copy the file
		ifstream source(filename, ios::binary);
		ofstream dest(destfile, ios::binary);

		dest << source.rdbuf();
		source.close();
		dest.close();
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}


bool CopyFile(char *srcfile, char * destfile)
{

	// if the file doesn't exist, flag it
	if ( !file_exists(srcfile)) return FALSE;

	try
	{

		// if there's an existing file, then kill it
		DeleteFile(destfile);

		// now copy the file
		ifstream source(srcfile, ios::binary);
		ofstream dest(destfile, ios::binary);

		dest << source.rdbuf();
		source.close();
		dest.close();
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}



/*
	RETURNS: TRUE on success
			FALSE othewise
*/

int CreateDirectory(char *path)
{

	if (file_exists(path) ) return TRUE;	// act like we created it

	int retval = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH | S_IXOTH);

	if (retval == 0) return TRUE;	// no errors

	return FALSE;

}


/*
	Delete a directory
	RETURNS: TRUE on success
			FALSE on failure
*/


int DeleteDirectory(char *path)
{
	sprintf(gen_buffer,"rm -rf %s\n",path);

	if (system(gen_buffer)==0)
		return TRUE;
	else
		return FALSE;

}




/*
	Delete a file
	RETURNS: TRUE on success
			FALSE on failure

*/


int DeleteFile(char *filename)
{

	// if the file doesn't exist, then act like we deleted it anyway

	if (!file_exists(filename))
		return TRUE;

	if (unlink(filename) == 0)
		return TRUE;
	else
		return FALSE;
}


// I've seen references to using the lighter access() fn instead of opening the file, however
// that methodology has portability issues as well as some timing issues. also, access() is a 
// POSIX std, not a C std
//
// returns TRUE if file exists
// FALSE otherwise

int file_exists(char * filename)
{
	FILE * fp;

	fp = fopen(filename, "r");
	if (fp)
	{
		fclose(fp);
		return TRUE;
	}

	return FALSE;
}



/*
	append *message to *filename file

	the "a" option says to append to a file, create the file if necessary

	RETURNS: OK or ERROR

*/

int WriteFile(char * filename, char * message)
{
    FILE *logfile;
    logfile=fopen(filename,"a");
    if (!logfile) return ERROR;
    fprintf(logfile,"%s\n",message);
	if ( ferror(logfile) )
		return ERROR;

    fclose(logfile);

	return OK;
}





/*
    create a nix timestamp as follows
    Sat Jan 27 10:32:12 2007

*/

char * TimeStamp(void)
{
    time_t ltime;       // calendar time
    ltime=time(NULL);   // get current cal time
    sprintf(tstamp,"%s",asctime( localtime(&ltime) ) );
	return &tstamp[0];
}



/*
	return EPOCH timestamp, eg. seconds since EPOCH

	NOTE: uintmax_t is defined in #include <stdint.h>


*/

uintmax_t EpochTime(void)
{
	time_t result;
	result = time(NULL);	// get seconds since EPOCH
	return (uintmax_t) result;
}



/*
	count the number of lines in the file

	if include_comments == TRUE, then include all lines in the count
	if include_comments == FALSE, then exclude blank lines and comments

	RETURNS:	OK or ERROR
*/

int CountFileLines(char *fname, int *count, bool include_comments=FALSE)
{
	#define MAX_LINE_LEN 255
	char buffer[MAX_LINE_LEN+1];
	FILE * myfile;
	(*count) = 0;

	myfile=fopen(fname,"r");
	if (!myfile)
	{
		printf("Unable to read file %s\n",fname);
		return ERROR;
	}

do
{
	if (fgets(buffer, MAX_LINE_LEN, myfile) == NULL) break; // read a line
	if (feof(myfile)) break;
	if (ferror(myfile))
	{
		fclose(myfile);
		return ERROR;
	}

	chomp(buffer); // strip CRLF and make ASCIZ
	trim(buffer);

	if (! include_comments)
	{
		if (buffer[0] == '#') continue; 		// skip comments
		if (buffer[0] == ';') continue;
		if (strlen(buffer) ==0) continue;	// skip if blank line
	}

	*count +=1;

} while (! feof(myfile));


	fclose(myfile);

	return OK;

}





/*
	everything exits through here
	this function is called at the end of main() in both production and versasafe

*/


void exit_cleanup(int exit_code)
{

	#if PGM_FLAVOR==PRODUCTION_PGM
		printf("Exiting VersaSafe, ver %d.%d.%d\r\n",MAJOR_VERSION, MINOR_VERSION, MICRO_VERSION );
	#endif

	#if PGM_FLAVOR==FLOWFORM_PGM
		printf("Exiting %s, ver %d.%d.%d\r\n",product_name,MAJOR_VERSION, MINOR_VERSION, MICRO_VERSION );
	#endif



	printf("Copyright 2017-%d FireKing Security Group, All Rights Reserved\r\n", current_year);
	printf("Author Gary Conway <gary.conway@fireking.com>\r\n");
//	printf("\r\n");
	CloseHelp();			// close down the help system


	string ret=GetLockDriverVersion();
	printf("Lock Driver Version %s\n",ret.c_str() );
	USB_shutdown();	// in usb_gateway.cpp


printf("Exit 15\n");



	if (countries != NULL)		delete[] countries;
printf("Exit 18, end of exit_cleanup()\n");
}










// ms delay

void delay(int dly)
{
	usleep(dly*1000);	// usleep works in micro seconds
}


/*
	print a string in hex
	this function is overloaded for char* and string types
*/

void phex(string msg)
{

    for (int n=0; n< msg.length(); n++)
        printf("%02X ",(unsigned char) msg[n]);
}


void phex(char *msg)
{

    for (int n=0; n< strlen(msg); n++)
        printf("%02X ",(unsigned char) msg[n]);
}



/*
	call once at program start with 0, call again a exit with 1
	get free memory information
	this function is used to help detect memory leaks

	int state:
				0 = get the free memory info and save it
				1 = get free memory and show this value as well as the saved value
*/


void ShowFreeMem(int state)
{
	#define MAX_LINE_LENGTH 100

	char * ptr;
	char filename[]="/proc/meminfo";
	char buffer[MAX_LINE_LENGTH];
	char temp[30];
	static char saved[30];

	FILE * memfile;

	memfile=fopen(filename,"r");
	if (!memfile)
	{
		printf("Unable to open mem file %s\n",filename);
		return;
	}

do
{
	if (fgets(buffer, MAX_LINE_LENGTH, memfile) == NULL) break; // read a line
	if (feof(memfile)) break;
	if (ferror(memfile))
	{
		fclose(memfile);
		return;
	}

	chomp(buffer); // strip CRLF and make ASCIZ
	trim(buffer);

/*
MemTotal:        2043300 kB
MemFree:          731060 kB
Buffers:          154292 kB
Cached:           572544 kB
SwapCached:            0 kB
Active:           737896 kB

*/


	ptr = strstr( buffer, "MemFree:");
 	if (ptr != NULL)
	{
		// found the string, now get the memory value
		ptr += 8;
		strcpy( temp, ptr);	// copy the memory number to temp
		chomp(temp);
		trim(temp);

		if (state == 0)
			strcpy( saved, temp);

		if (state == 1)
		{
			printf("Free Memory:  Start: %s   Exit: %s\r\n\r\n", saved, temp);
		}

		fclose(memfile);
		return;
	}



} while (! feof(memfile));


	fclose(memfile);


}



void GetDate(void)
{
	time_t t = time(0);	// get current time
	struct tm * now = localtime(&t);

	current_year = now->tm_year + 1900;
	current_month = now->tm_mon+1;
	current_day = now->tm_mday; 
}







void ShowLinuxVersion(void)
{

	#define BSIZE 1000
	FILE * nix;
	char * buff= new char[BSIZE];
	char run_string[40];
	int status=ERROR;

	// create the system string of the command to run
	// get the kernel version et al
	sprintf(run_string,"uname -siorvmp");

	// popen() creates a pipe so we can read the output
	if ( !(nix = popen(run_string,"r")))
		return;
	bzero(buff,BSIZE);

	// read and process a line from the results of the external FTP command
	if ( fgets(buff,BSIZE,nix) != NULL)
		printf("%s",buff);

	pclose(nix);


	// create the system string of the command to run
	// get the Ubuntu version
	sprintf(run_string,"cat /etc/issue");

	// popen() creates a pipe so we can read the output
	if ( !(nix = popen(run_string,"r")))
		return;
	bzero(buff,BSIZE);

	// read and process a line from the results of the external FTP command
	if ( fgets(buff,BSIZE,nix) != NULL)
	{
		char * ptr = strstr(buff,"\\");
		*ptr='\0';
		printf("%s\n",buff);
	}

	pclose(nix);



	delete[] buff;

}




// create a timestamp from the current time
// format is "2012-08-12 01:56:30.000"

void MakeTimeStamp(char * timeStr)
{
	time_t current;
	struct tm * timeptr;
	current = time(NULL);
	timeptr = localtime(&current);
	sprintf(timeStr,"%d-%02d-%02d %02d:%02d:%02d.000",
		timeptr->tm_year+1900,
		timeptr->tm_mon+1,
		timeptr->tm_mday,
		timeptr->tm_hour,
		timeptr->tm_min,
		timeptr->tm_sec);
}






/*
	check to see if a directory or filepath exists
	RETURNS: OK if directory exists
			ERROR if not

*/

int FileDirExists(char * dirpath)
{
	struct stat sb;

	if (stat(dirpath, &sb) < 0)
		return ERROR;

	return OK;
}




/*
	Create a directory
	RETURNS: OK on success
			ERROR on failure
*/


int MakeDir(char * dirpath)
{
	int res;
	res=mkdir(dirpath,755);
	if (res != 0)
		return ERROR;

	return OK;

}






