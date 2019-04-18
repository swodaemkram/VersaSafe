
/*
	Program: VersaSafe
	Module: versasafe.cpp (primary module)
	Author: Gary Conway <gary.conway@fireking.com>
	Created: 12-1-2017
	Updated:
	Language: C,C++ (this module is C++ compilable and linkable with C libs)
	Version: 1.00


	GTK windowing notes:

	the x button calls the widget->delete_event, so if you have a CLOSE button, and the x button is to be visible
	have the x button signal the same fn as the CLOSE button.

	Note: writing to hidden labels, etc. causes errors to be sent to stdout


	MEM LEAKS
	---------

	if foo isn't a toplevel window, then the following creates a mem leak
	foo = gtk_foo_new();
	gtk_widget_destroy(foo);

	rather, do it this way...

	create foo and a reference to it...
	foo = gtk_foo_new()
	g_object)ref(foo);
	gtk_object_sink(foo);


	then releasing foo...
	gtk_widget_destroy(foo);
	g_object_unref(foo);

	if the above widget was added into a container, then simply do gtk_widget_destroy(foo);




	NOTE: the Makefile sets the variable PGM_FLAVOR as a compiler flag, therefore we have
	access to the variable when compiling and can configure the compilation for either
	PRODUCTION or VERSASAFE as follows...

	#if PGM_FLAVOR==PRODUCTION
	#endif

	#if PGM_FLAVOR==VERSASAFE
	#endif

	... where... PRODUCTION and VERSASAFE are defined in flow.h



	Source Directory Structure:


language files

need to be installed on machine


	autoconf - permits automatic configuration of software installation
	automake - examines C files, determines their dependencies and creates the Makefile

	Flow Font (Franklin Gothic Medium)
	Flow Colors (R,G,B)

		Teal 								Gray
		100%	(0,146,159)					(65,83,93)
		80%		(0,161,174)					(100,116,127)
		60%		(94,179,190)				(132,146,156)
		40%		(146,201,208)				(167,179,188)
		20%		(96,224,228)				(200,214,220)


	==============================================================================================
	gtkimageviewer compilation

	run...
	./configure
	then edit the configure file and change ac_default as shown below


	5-9-2013 DO NOT MAKE THE FOLLOWING CHANGE
	#
	# Initializations.
	#
	#GRC changed the following line
	ac_default_prefix=/usr
	#ac_default_prefix=/usr/local
	ac_clean_files=
	ac_config_libobj_dir=.

	the change above is necessary AFTER running ./configure because /usr/local is wrong
	and must be changed to /usr

	then run...
	make
	make install

	make code changes in the gtk-image-viewer-fixed.c file
	then recompile using...
	make
	make install
	==============================================================================================




	MILESTONES
	----------
	12-7-2017	first compile, bare skeleton
	12-7-2018	lock driver working
	1-15-2019	lock times window completed
	4-1-2019	working with mark

	THREADS:

		http://developer.gnome.org/glib/unstable/glib-Threads.html



	Memory Usage
------------------------------------------------------------------------------



=============================
building a combobox in GLADE
=============================


1. add the combobox widget to the application
2. on the combobox properties window, add a liststore model
3. when you click on the combobox in main window, a new tool (pencil) appears on the top menu bar.. click that...
4. click the hierarchy tab, then click ADD to add a CellRendererText widget to the liststore
5. in the liststore, add a column TYPE: gchararray, NAME: combotext
6. in the CellRendererText, under "Text" add "combotext" to the entry
7. add the combo widget name to the GTK widget structure




*/

using namespace std;


//================================================================================================
// 					MACRO DEFINITIONS TO COLOR THE COMPILATION
//================================================================================================


// the following enable LOCK or UTD class initialization
//#define LOCKS
//#define UTD

//#define PRODUCTION
#define SANDBOX


#ifdef PRODUCTION
	#define THREADS				// define to use threading
	#define TIME_SLICE 100		// frequency in ms that gtk_main() calls our time slice
	#define CHECK_CONNECTIONS	// define to allow checking TCP connections
	#define TRAP_SEGFAULT
	#define ENABLE_TIMESLICE
#endif


#ifdef SANDBOX
	#define DISABLE_LOGIN		// define to disable the login window
	#define THREADS				// define to use threading
	#define TIME_SLICE 100		// number of ms for our time slice taken from gtk_main()
	#define CHECK_CONNECTIONS	// define to allow checking TCP connections
    #define ENABLE_TIMESLICE

//	#define TRAP_SEGFAULT
	#define DBG(a) 	// macro for debug printing
#endif






//================================================================================================
//================================================================================================


 


#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <unistd.h>	// for geteuid()
//#include <gtkimageviewer-2.0/gtk-image-viewer.h>
//#include <cairo.h>

//#include <libusb-1.0/libusb.h>
#include "tinyxml/tinyxml.h"
#include "mysql.h"

// allowable for GTK+3
/*
#include <gtk/gtk.h>
#include <gtk/gktunixprint.h>	// low-level UNIX-specific print functions
#include <gdk/gdk.h>
#include <gdk/gdkx.h>		// for X11-specific functions
#include <gdk/gdkwin32.h>	// windows-specific functions
*/

/*
// mysql connector stuff (not used)
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
*/


#include "queue.h"
#include "hdr/fire.h"
#include "hdr/config.h"
#include "usb_gateway.inc"
#include "hdr/usb_gateway.h"



//mysql  Ver 14.14 Distrib 5.7.20, for Linux (x86_64) using  EditLine wrapper

// xml file produced by glade
#define XML_PORTRAIT "xml/port.glade"
//#define XML_LANDSCAPE "xml/land.glade"
#define XML_LANDSCAPE "xml/xlc.glade"

char XML_FILE[50];

bool ConfigSetup(bool silent);
void PrintConfig(void);


// color scheme resource file
#define RES_FILE "../res/flowformrc"


// pid.cpp
bool CheckRunning(int pid_ID);		// check to see if flowform is running, if so, return TRUE
bool createPIDfile(int pid_ID);		// create the PID file
bool removePIDfile(int pid_ID);		// remove the PID file


// help.cpp
#include "help.inc"

//usb
#include "usb.inc"

// segfault.cpp
extern void TrapSegFault(void);
extern void TrapBrokenPipe(void);


// internals for help functions



/*
	Instructions for Adding a new section to the help file
	1. add a new entry to the enum{} below
	2. add a new entry in help_index[] below. make sure the section_tag matches the section tag
		in the XML help file EXACTLY.
	3. add the new help file section to the XML help file. note the "name" atrribute is the display name used

		<new_section name="New Section">
			<Page>
			<Title></Title>
			<Header></Header>
			<Body></Body>
			<Footer></footer>
			</Page>
		</new_section>
*/

enum
{
	HELP_CONTENTS=0,
	HELP_MAIN_WINDOW,
	HELP_CONFIG_WINDOW,
	HELP_ENTRY_COUNT
};

// HELP_ENTRY_COUNT+1 = number of entries in the above struct

// the following structure allows finding the
// help_window number if we know the tag and vice-versa
struct hindex
{
	string section_tag;
	int help_window_number;
};

// the section_tags are the tags found in the help XMLfile
// denoting the various sections of the help document
// these must be exact case matches

hindex help_index[] =
{
	{ "contents", HELP_CONTENTS  },
	{ "main_window", HELP_MAIN_WINDOW  },
	{ "config_window", HELP_CONFIG_WINDOW  }
};




#include "trim.inc"

#include "dbf.inc"

// common.cpp
#include "common.inc"

// logs.cpp
#include "logs.inc"

// ip_module.cpp
#include "ip_module.inc"


// locale.cpp
#include "locale/locale.h"
#include "locale.inc"

// resource.cpp
#include "resource.inc"

// in config.h


// in ftp.cpp
extern int FTP_upload(char * ip);
extern int FTP_download(char * ip);
extern int FTP_upload_controller_program(char *ip);

// in dbf.cpp
extern bool ConnectDBF(void);
extern string TestQuery(void);


/*
	user codes:
	0 = bad login
	1 = normal user		pw: flow2012
	2 = flow tech user	pw: flowtech
	3 = god	pw: viper
*/
#define NO_USER -1
#define BAD_LOGIN 0
#define NORMAL_USER 1
#define FLOW_TECH 2
#define GOD_MODE 3
int user_code=NO_USER;		// default until login complete


OperationMode OpMode;

void SetScreenSizes(void);
void GetScreenRes(void);
struct
{
    int horiz;
    int vert;
} screenres;



#include "hdr/gtkwidgets.h"

// our gtk builder object, used to read in and interpolate the XML widgetry file
// we then use the builder object to set up various parameters for all of our objects
// which are all containted within the builder object

GtkBuilder *builder;

//void UpdateUTDInventory(int nodeid);

void SetLabels(void);

gint screen_width;
gint screen_height;

int help_window=0;
bool STARTUP_COMPLETE=FALSE;
bool SHUTTING_DOWN=FALSE;

// flow colors
GdkColor flow_color;
GdkColor flow_teal;
GdkColor flow_gray;
GdkColor high_color;
GdkColor hover_color;
GdkColor flow_orange;
GdkColor flow_white;
GdkColor flow_light_teal;

bool time_slice_enabled=FALSE;



struct timelock
{
    int active=FALSE;
    int lock_index;     // index into locks[] struct for usb_gateway
    int unlock_day;
    int unlock_hour;    // must be in mil format
    int unlock_minute;
    int lock_day;
    int lock_hour;      // must be mil format
    int lock_minute;
    bool locked=TRUE;
    int etime;          // lock delay for this lock
    int seconds;        // used to keep track of how long the lock has been locked, so we know when to send another UNLOCK cmd, s$
                        // doesn't automatically re-lock
};

timelock OUTTER_LOCK_TL;
timelock INNER_LOCK_TL;
timelock SHUTTER_LOCK_TL;
timelock SIDECAR_LOCK_TL;
timelock BASE_LOCK_TL;

//lockstruc stuff

char selected_lock[30]="outterdoor";
char selected_day[10]="Sun";

typedef struct
{
    char lockname[11];
    int lock_enable;
    int alldayssame;

    int sun_enable;
    int mon_enable;
    int tue_enable;
    int wed_enable;
    int thu_enable;
    int fri_enable;
    int sat_enable;

    char sun_lock[6];
    char sun_unlock[6];
    char mon_lock[6];
    char mon_unlock[6];
    char tue_lock[6];
    char tue_unlock[6];
    char wed_lock[6];
    char wed_unlock[6];
    char thu_lock[6];
    char thu_unlock[6];
    char fri_lock[6];
    char fri_unlock[6];
    char sat_lock[6];
    char sat_unlock[6];

} lockstruc;


lockstruc lockrec;

// NOTE: the following array is filled in by GetLockNames()
// and can be indexed via OUTTER_LOCK,INNER_LOCK etc.
string lockrecords[MAX_LOCKS]={"outterdoor","innerdoor","shutter","sidecar","base"};


//GRCDEF private declarations
void KillConnections(void);
void DisableTimeSlice(void);
void EnableTimeSlice(void);
gint time_slice(gpointer data);
void SetMainScreenFont(void);
void SetMainScreenTitle(void);

void ShowMainWindow(void);
void ShowLoad(void);
void ShowConfig(void);
void ShowMaint(void);
void ShowUTDMaint(void);
void ShowMEIMaint(void);
void ShowLogin(void);


void Get_UTD_Data(void);
void GetLangs(void);


void SetLockTimerLabels(char * lockname);
void StoreLockTime(char * hourmin, bool islock, bool ishour);
void ReadLockTimes(void);
void ReadLockTimes2(lockstruc lck,string sel_lock);
void SetTimeCombos(void);

string GetLockHour(void);
string GetLockMin(void);
string GetUNLockHour(void);
string GetUNLockMin(void);
string lockhrmin;
string unlockhrmin;
void WriteLockTimes(void);
void SetLockUnlockTimes(string locktime, string unlocktime);
void SetConfigLocks(void);
void ConfigLocksInDBF(void);
void GetLockPorts(void);
void SetConfigActiveBtns(void);
string bool_to_string(bool val);
void  GetLockNames(void);


string Markup(string str);
void MarkupBody(void);
//extern "C"
//static void UpdateUTDInventory(int nodeid);




typedef struct
{
    string outter;
    string inner;
    string shutter;
    string sidecar;
    string base;
} lock_menu_items;


lock_menu_items lock_menu;




// config vars
char screenOrientation[50]="landscape";

struct
{
    char screentxt[20];

	// dbf
    char iptxt[20];
    char usertxt[20];
    char pwtxt[20];
    char dbftxt[30];
    char porttxt[10];


    // devices
    char validator1[10];
    char validator2[10];
    char ucd1[10];
    char ucd2[10];
	char utd[10];
    char outterlock[10];
    char innerlock[10];
    char shutterlock[10];
    char sidecarlock[10];
    char baselock[10];
    char api[10];
    char recycler[10];
    char printer[10];
    char tampersw[10];


    // logs
    char logdbf[10];
    char logfile[10];
    char logremotedbf[10];

    // remote
    char clientportal[10];
    char maintportal[10];
    char dbfportal[10];

} cfg;





/*
	cleanup here.. free mem etc.
*/

void local_cleanup(void)
{

	KillConnections();


}



//extern "C" void on_login_window_destroy(GtkObject *object, gpointer data)
extern "C" void on_login_window_destroy()
{

	local_cleanup();	// in common.cpp
	gtk_main_quit();
}






// it all starts right here, since the white book, this has been the entry point for every c program ever written
// and VersaSafe is no different

int main(int argc, char *argv[])
{

	GetScreenRes();
/*
	printf("horiz: %d\n",screenres.horiz);
	printf("vert: %d\n",screenres.vert);
	exit(0);
*/


/*
//    getStatus() returns OK 00 t=0;
	string stat="OK 00 t=400;";
    int mtimer =stoi(stat.substr(8) );
	printf("mtimer: %d\n",mtimer);
	exit(0);
*/


//InitEventDriver();//exit(0);


// we must run as root
if(geteuid() != 0)
{
	printf("\n\n");
	printf("%s must be run as ROOT\n\n",product_name);
	exit(0);
}


	// this must happen BEFORE the call to GeneralSetup()
	GetLockPorts();


	string txt;
	InitDBF(&localDBF);

	TrapSegFault();
	TrapBrokenPipe();

initUSB(false);	// in usb.cpp

printf("user: %s\n",localDBF.user);
	ConnectDBF(&localDBF);
	char query[]="SELECT msg,user FROM test";
	int result =  QueryDBF(&localDBF,query);
	int numrows = GetRow(&localDBF);	// populate gen_buffer
	printf("Data Returned: %s\n",localDBF.row[1]);


Get_UTD_Data();
GetLangs();
//exit(0);

/*
	// returns TRUE If VersaSafe is already running
	if ( CheckRunning(VERSASAFE_PID) )
	{
		printf("%s is already running, exiting\r\n\r\n",product_name);
		return(0);
	}


	if ( !createPIDfile(VERSASAFE_PID) )		// create the PID file for VersSafe
	{
		printf("Unable to create a PID file for %s, exiting\r\n\r\n",product_name);
		return(0);
	}
*/
	ShowFreeMem(0);	// get startup free memory


/*
//#ifdef THREADS
	// secure GLIB


	// prevents calling g_thread_init() if it has been initialized already
	if( ! g_thread_supported() )
	    g_thread_init( NULL );

	// secure GDK
	gdk_threads_init();

	// obtain GDK's global lock
	gdk_threads_enter();
//#endif
*/
	// create a local pointer to the widget object Appwidget
	AppWidgets *app = g_slice_new(AppWidgets);
	app_ptr = app;	// save a global pointer

//	inject_rc_file(RES_FILE);
	// initialize GTK
	gtk_init(&argc, &argv);

	//	Set_GTK_Message_Handler();	// take over the GTK message handler



//===========================================================

//=========================
//	NON-GTK SETUP
//=========================


//NOTE: the order in the following calls is important

    GetLockPorts();		// this must happen BEFORE the call to GeneralSetup()
						// GetLockPorts() shells to run findlocks script
						// listing all connected FKI_Security_Group_LLC devices
						// and the port they are connected on
//TODO serialports[] must be mapped to the connected locks
// iow, the ports listed in locks[].port must be written to serialports[]
// in OUTTER_LOCK,INNER_LOCK,SHUTTER_LOCK,SIDECAR_LOCK,BASE_LOCK order
// the portname for OUTTER_LOCK will go in serialports[0]
// INNER_LOCK to serialports[1]
// etc.
// example
// serialports[0]="/dev/ttyACM0"
// serialports[1]="/dev/ttyACM1"
// unused entries to have NULL
// there are MAX_LOCKS entries
// etc.

	GetLockNames();				// retrieve lockname field from locks table
	GeneralSetup();				// in common.cpp

	if (!ConfigSetup(false) )		// in this module
	{
		local_cleanup();
		exit_cleanup(0);		// global cleanup in common.cpp
		exit(1);
	}

	// ConfigSetup() must  have already been run before the following functions
    ConfigLocksInDBF(); 	// enable appropriate locks in DBF and locks[] object
    ReadLockTimes();        // get lock/unlock times from database

#ifdef LOCKS
	USB_init();				// configure/connect the locks in locks[] object
#endif

//===========================================================

//=======================

// driver tests go here

if ( strcmp(cfg.utd,"enabled")==0)
{

#ifdef UTD
	Init_D8C();     // init VEND board driver (in usb_gateway)
#endif
//	Unload_D8C(1);
}

/*
=============================================================================================
Initiate the MEI validator in the USB Gateway if Enabled
=============================================================================================
*/
if ( strcmp(cfg.validator1,"enabled")==0)
{
	init_mei();    //init MEI validator (in usb_gateway)
}
/*
=============================================================================================
end of Initiating the MEI validator in the USB Gateway if Enabled
=============================================================================================
*/


//=======================



//=========================
//	GTK SETUP
//=========================

	GError *err = NULL;


	int gtk_builder_ret;
	// create a gtk builder object for reading and interpreting the XML file
	builder = gtk_builder_new();
	// returns 0 on failure and err is set with a message
	if ( strcmp(screenOrientation,"landscape") ==0)
		strcpy(XML_FILE,XML_LANDSCAPE);
	else
		strcpy(XML_FILE,XML_PORTRAIT);

	gtk_builder_ret = gtk_builder_add_from_file(builder,XML_FILE,&err);
	if (gtk_builder_ret == 0)
	{
		g_warning("%s",err->message);
		g_error_free(err);
		g_slice_free(AppWidgets,app);
//		#ifdef THREADS
//			gdk_threads_leave();	// release the GTK global lock
//		#endif
		return 1;
	}
printf("XML is read, ret:%d\n",gtk_builder_ret);


#include "hdr/gtkwidget_app.h"


	gtk_builder_connect_signals(builder,app);
	g_object_unref(G_OBJECT(builder));


	// setup a callback timer, gtk_main() will call this function every 100ms
	gint app_slice = g_timeout_add(TIME_SLICE,time_slice,NULL);
	// NOTE: app_slice can be used to remove the callback
	//g_source_remove(app_slice);


/*
	gtk_rc_parse_string("style 'flow_color' {"
		" fg[NORMAL] = { 0,146,159 }"
		"}"
		"widget 'logo_label' style:highest 'flow_color'"
	);
*/


#ifdef ENABLE_TIMESLICE
	EnableTimeSlice();
#endif



#ifdef DISABLE_LOGIN
	user_code=GOD_MODE;
#else
	user_code = NO_USER;
	gtk_widget_show(app->login_dialog);
#endif


//	ShowMainWindow();
//	ShowLogin();
//	gtk_widget_show_all(app->main_screen);
//	gtk_widget_hide(app->popup_window);




	// log system startup
	sprintf(gen_buffer,"%s started",product_name);
	WriteSystemLog(gen_buffer);

    // HELP setup
    help_window = HELP_MAIN_WINDOW;
    //TODO must setup help file name with the other text files depending upon locale
    string help_filename="../locale/english_help.xml";
    SetHelpFileName(help_filename);


	SetScreenSizes();
	SetLabels();
	SetMainScreenFont();
	SetMainScreenTitle();




	#ifdef FULLSCREEN
//		gtk_window_fullscreen( GTK_WINDOW(app_ptr->main_screen) );
	#endif

    ShowLogin();


	// enter the GTK event loop
	STARTUP_COMPLETE=TRUE;
	gtk_main();

	SHUTTING_DOWN=TRUE;
	delay(100);





//	WriteConfig( (char *) "/home/garyc/Desktop/C_source/FlowForm/gary.txt");
//	WriteConfig( (char *) config_file);




//=========================
//	EXIT BLOCK
//=========================



	// log system exit
	printf("\n\n");
	printf("=====================================================================\r\n");
	sprintf(gen_buffer,"Exiting %s ver %d.%d.%d",product_name,MAJOR_VERSION, MINOR_VERSION, MICRO_VERSION );
	WriteSystemLog(gen_buffer);
	ShowLinuxVersion();

	int txml_major_version;
	int txml_minor_version;
	int txml_patch_version;
	GetXMLVersion( &txml_major_version, &txml_minor_version, &txml_patch_version);

	local_cleanup();		// local cleanup (must be done first so connections are still active)
	exit_cleanup(0);		// global cleanup in common.cpp


extern void getUSBversion(void);
	getUSBversion();	// populates gen_buffer
	string d8cstr =Get_d8_driver();

//	printf("\r\n");
	printf("Source Language: C/C++ --export-dynamic\r\n");
	printf("Using GTK/GDK+ libraries v.%d.%d.%d\r\n",gtk_major_version,gtk_minor_version,gtk_micro_version);
	printf("Using TinyXML Library v.%d.%d.%d\r\n",txml_major_version,txml_minor_version,txml_patch_version);
	printf("TCP/UDP: Internal proprietary\r\n");
	printf("Database management: MySql v.%s\r\n",LIBMYSQL_VERSION);
	printf("USB Drivers: %s\r\n",gen_buffer);
	printf("I/O Drivers: Internal proprietary\r\n");
	printf("D8C Driver: v %s\n",d8cstr.c_str());
	printf("FTP: External\r\n");
	printf("======================================================================\r\n");


	printf("\r\n");
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");

	removePIDfile(VERSASAFE_PID);

	ShowFreeMem(1);	// display startup and finish free memory
	return 0;

}



/*
	read the DBF:locks and fill in the following array
	string lockrecords[MAX_LOCKS]={"outterdoor","innerdoor","shutter","sidecar","base"};

*/
void  GetLockNames(void)
{
	char query[50]="SELECT lockname FROM locks ORDER BY id;";
    int result =  QueryDBF(&localDBF,query );
    int numrows=GetRow(&localDBF);;
	int num;

	for (int n=0; n < numrows; n++)
	{
		printf("%s\n",localDBF.row[0]);
		lockrecords[n] = (string) localDBF.row[0];

		num = GetRow(&localDBF);
		if (num == 0) break;

	}




}




string bool_to_string(bool val)
{
    if (val) return "1";
    return "0";
}




void error_exit(void)
{
	gtk_main_quit();
}


void DisableTimeSlice(void)
{
	time_slice_enabled=FALSE;
}

void EnableTimeSlice(void)
{
	time_slice_enabled=TRUE;
}





typedef struct
{
    string port;
    string vendor;
	bool enabled;
} ulock;

extern ulock usblocks[MAX_LOCKS];		// in usb.cpp
//extern int lock_map[MAX_LOCKS];			// in usb.cpp
extern string serialports[MAX_LOCKS];	// in usb.cpp

void GetLockPorts(void)
{
	// lock_map has the indices into the serialports[] array
	//  arranged as follows...
	// OUTTER_LOCK
	// INNER_LOCK
	// SHUTTER_LOCK
	// SIDECAR_LOCK
	// BASE_LOCK


	// locate all USB FKI locks and their port
    FindLocks();    // in usb.cpp

	for (int n=0; n < MAX_LOCKS; n++)
	{
		if (!usblocks[n].enabled) continue;
		printf("LOCK: %d Port: %s\n",n,usblocks[n].port.c_str() );
	}


}


// LOCKCONFIG SECTION
//=============================================================================================




bool changing_day=FALSE;


/*
	check the current time against the lockrec times for all enabled days
	to see if the lock should be locked or unlocked

timelock struct
{
    int active=FALSE;
    int lock_index;     // index into locks[] struct for usb_gateway
    int unlock_day;
    int unlock_hour;    // must be in mil format
    int unlock_minute;
    int lock_day;
    int lock_hour;      // must be mil format
    int lock_minute;
    bool locked=TRUE;
    int etime;          // lock delay for this lock
    int seconds;        // used to keep track of how long the lock has been locked, so we know when to send another UNLOCK cmd, s$
                        // doesn't automatically re-lock
}

timelock OUTTER_LOCK_TL;
timelock INNER_LOCK_TL;
timelock SHUTTER_LOCK_TL;
timelock SIDECAR_LOCK_TL;
timelock BASE_LOCK_TL;

also
int current_hour;	0-23
int current_minute;	0-59
int current_day;	0-7 (sun=0)

cfg struct
   char outterlock[10];
    char innerlock[10];
    char shutterlock[10];
    char sidecarlock[10];
    char baselock[10];

string lockrecords[]={"outterdoor","innerdoor","shutter","sidecar","base"}

1. pull record for enabled lock
2. check if its lock or unlock time
3. lock or unlock as necessary

*/
//GRC
void CheckLockTimes(void)
{
	string sel_lock;

	lockstruc lrec;
	timelock tl;

	for (int n=0; n < MAX_LOCKS; n++)
	{
		// get the locknames
		switch(n)
		{
		case 0:	// outterdoor
			if ( strcmp(cfg.outterlock,"enabled")==0)
				sel_lock=lockrecords[n];
				tl=OUTTER_LOCK_TL;
			break;
		case 1:	// innerdoor
            if ( strcmp(cfg.innerlock,"enabled")==0)
                sel_lock=lockrecords[n];
				tl=INNER_LOCK_TL;
            break;
		case 2:	// shutter
            if ( strcmp(cfg.shutterlock,"enabled")==0)
                sel_lock=lockrecords[n];
				tl=SHUTTER_LOCK_TL;
            break;
		case 3:	// sidecar
            if ( strcmp(cfg.sidecarlock,"enabled")==0)
                sel_lock=lockrecords[n];
				tl=SIDECAR_LOCK_TL;
            break;
		case 4:	//base
            if ( strcmp(cfg.baselock,"enabled")==0)
                sel_lock=lockrecords[n];
				tl=BASE_LOCK_TL;
            break;

		}

		ReadLockTimes2(lrec,sel_lock);		// populate lrec with the database record for this lock

		if (!lrec.lock_enable) continue;	// if this lock isnt enabled, then skip it

		switch(current_day)
		{
		case 0:	//sun
			if (lrec.sun_enable)
			{
				// check the lock/unlock times for sunday

/*
    int active=FALSE;
    int lock_index;     // index into locks[] struct for usb_gateway
    int unlock_day;
    int unlock_hour;    // must be in mil format
    int unlock_minute;
    int lock_day;
    int lock_hour;      // must be mil format
    int lock_minute;
    bool locked=TRUE;
    int etime;          // lock delay for this lock
    int seconds;        // used to keep track of how long the lock has been locked, so we know when to send another UNLOCK cmd, s$
*/
					tl.lock_index=n;
					tl.unlock_day=current_day;
					vector <string> tm=split(lrec.sun_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.sun_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
					tl.locked=TRUE;
					tl.active=TRUE;
					tl.seconds=0;
			}
			break;
		case 1:	//mon
			if (lrec.mon_enable)
			{
				// check lock/unlock times for monday
					tl.lock_index=n;
					tl.unlock_day=current_day;
                    vector <string> tm=split(lrec.mon_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.mon_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
                    tl.locked=TRUE;
					tl.active=TRUE;
                    tl.seconds=0;
			}
			break;
		case 2:	// tue
			if (lrec.tue_enable)
			{
				// check lock/unlock times for tues
					tl.lock_index=n;
					tl.unlock_day=current_day;
                    vector <string> tm=split(lrec.tue_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.tue_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
                    tl.locked=TRUE;
					tl.active=TRUE;
                    tl.seconds=0;
			}
			break;
		case 3: // wed
           if (lrec.wed_enable)
            {
                // check lock/unlock times for wed
					tl.lock_index=n;
					tl.unlock_day=current_day;
                    vector <string> tm=split(lrec.wed_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.wed_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
                    tl.locked=TRUE;
                    tl.active=TRUE;
                    tl.seconds=0;
            }
            break;
		case 4: //thu
           if (lrec.thu_enable)
            {
                // check lock/unlock times for thu
					tl.lock_index=n;
					tl.unlock_day=current_day;
                    vector <string> tm=split(lrec.thu_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.thu_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
                    tl.locked=TRUE;
                    tl.active=TRUE;
                    tl.seconds=0;
            }
            break;
		case 5: //fri
           if (lrec.fri_enable)
            {
                // check lock/unlock times for fri
					tl.lock_index=n;
					tl.unlock_day=current_day;
                    vector <string> tm=split(lrec.fri_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.fri_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
                    tl.locked=TRUE;
                    tl.active=TRUE;
                    tl.seconds=0;
            }
            break;
		case 6: // sat
           if (lrec.sat_enable)
            {
                // check lock/unlock times for sat
					tl.lock_index=n;
					tl.unlock_day=current_day;
                    vector <string> tm=split(lrec.sat_unlock,":");
					tl.unlock_hour= stoi(tm[0]);
					tl.unlock_minute= stoi(tm[1]);

					tl.lock_day=current_day;
                    vector <string> tm1=split(lrec.sat_lock,":");
					tl.lock_hour= stoi(tm1[0]);
					tl.lock_minute= stoi(tm1[1]);
                    tl.locked=TRUE;
                    tl.active=TRUE;
                    tl.seconds=0;
            }
            break;

		} //end case

		// set the lock delay (this gets sent to the lock for the auto-relock time)
	    tl.etime=GetLockDelay(tl.lock_index);



	}// end for


}



/*
    get the lock and unlock times from the DBF
    for the currently selected lock
    and populate the lockrec struct
*/
void ReadLockTimes2(lockstruc lck,string sel_lock)
{
    char query[200];
    sprintf(query,"SELECT *  FROM locks WHERE lockname='%s' AND enabled='1';",sel_lock.c_str() );
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer

    bzero(lck.lockname,11);
    bzero(lck.sun_lock,6);
    bzero(lck.sun_unlock,6);
    bzero(lck.mon_lock,6);
    bzero(lck.mon_unlock,6);
    bzero(lck.tue_lock,6);
    bzero(lck.tue_unlock,6);
    bzero(lck.wed_lock,6);
    bzero(lck.wed_unlock,6);
    bzero(lck.thu_lock,6);
    bzero(lck.thu_unlock,6);
    bzero(lck.fri_lock,6);
    bzero(lck.fri_unlock,6);
    bzero(lck.sat_lock,6);
    bzero(lck.sat_unlock,6);


    strncpy(lck.lockname,localDBF.row[0],10);
    strncpy(lck.sun_lock,localDBF.row[1],5);
    strncpy(lck.sun_unlock,localDBF.row[2],5);
    strncpy(lck.mon_lock,localDBF.row[3],5);
    strncpy(lck.mon_unlock,localDBF.row[4],5);
    strncpy(lck.tue_lock,localDBF.row[5],5);
    strncpy(lck.tue_unlock,localDBF.row[6],5);
    strncpy(lck.wed_lock,localDBF.row[7],5);
    strncpy(lck.wed_unlock,localDBF.row[8],5);
    strncpy(lck.thu_lock,localDBF.row[9],5);
    strncpy(lck.thu_unlock,localDBF.row[10],5);
    strncpy(lck.fri_lock,localDBF.row[11],5);
    strncpy(lck.fri_unlock,localDBF.row[12],5);
    strncpy(lck.sat_lock,localDBF.row[13],5);
    strncpy(lck.sat_unlock,localDBF.row[14],5);

    // lock enable/disabled
    lck.lock_enable= atoi(localDBF.row[15]);

    // get active status for each day
    lck.sun_enable=atoi(localDBF.row[16]);
    lck.mon_enable=atoi(localDBF.row[17]);
    lck.tue_enable=atoi(localDBF.row[18]);
    lck.wed_enable=atoi(localDBF.row[19]);
    lck.thu_enable=atoi(localDBF.row[20]);
    lck.fri_enable=atoi(localDBF.row[21]);
    lck.sat_enable=atoi(localDBF.row[22]);

    lck.alldayssame= atoi(localDBF.row[23]);

}







/*

	update the lock config display

	hrmin is the new hour or minute value to store in lockrec

	islock is a bool TRUE = set LOCK parm
					FALSE = set UNLOCK parm

	ishour is a bool TRUE = setting the HOUR
					FALSE = setting the MIN

*/

void StoreLockTime(char * hourmin, bool islock, bool ishour)
{
	string msg;
	int dayindex=0;

	// don't do this update when we are changing day
	if (changing_day) return;

    char dbfstr[10];
    bzero(dbfstr,10);

	msg = getMessage(51,FALSE);	// Sun
	if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=0;
		if (islock)
			strncpy(dbfstr,lockrec.sun_lock,5);
		else
			strncpy(dbfstr,lockrec.sun_unlock,5);
    msg = getMessage(52,FALSE);		//Mon
    if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=1;
        if (islock)
            strncpy(dbfstr,lockrec.mon_lock,5);
        else
            strncpy(dbfstr,lockrec.mon_unlock,5);

    msg = getMessage(53,FALSE);	// Tue
    if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=2;
        if (islock)
            strncpy(dbfstr,lockrec.tue_lock,5);
        else
            strncpy(dbfstr,lockrec.tue_unlock,5);

    msg = getMessage(54,FALSE);	//Wed
    if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=3;
        if (islock)
            strncpy(dbfstr,lockrec.wed_lock,5);
        else
            strncpy(dbfstr,lockrec.wed_unlock,5);

    msg = getMessage(55,FALSE);	// Thu
    if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=4;
        if (islock)
            strncpy(dbfstr,lockrec.thu_lock,5);
        else
            strncpy(dbfstr,lockrec.thu_unlock,5);

    msg = getMessage(56,FALSE);	// Fri
    if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=5;
        if (islock)
            strncpy(dbfstr,lockrec.fri_lock,5);
        else
            strncpy(dbfstr,lockrec.fri_unlock,5);

    msg = getMessage(57,FALSE);	// Sat
    if (strcmp(selected_day,msg.c_str() )==0)
		dayindex=6;
        if (islock)
            strncpy(dbfstr,lockrec.sat_lock,5);
        else
            strncpy(dbfstr,lockrec.sat_unlock,5);



//	strncpy(dbfstr,localDBF.row[index],5);		// get the hour:min pair

printf("mystr: %s\n",dbfstr);
	vector <string> dbf=split(dbfstr,":");
	char newstr[10];
	bzero(newstr,10);

	if (ishour)
	{
		strcpy(newstr,hourmin);	// copy the new hr
		strcat(newstr,":");
		strcat(newstr,dbf[1].c_str());	// add in the old min
	}
	else
	{
        strcpy(newstr,dbf[0].c_str());   // add in the old hour
        strcat(newstr,":");
		strcat(newstr,hourmin );	// add in new min
	}

//printf("newstr: %s\n",newstr);


// newstr has the newly compiled string for lockrec sun_lock or sun_unlock etc

if (islock)
{
	switch(dayindex)
	{
	case 0:	//sun
		strncpy(lockrec.sun_lock,newstr,5);
		break;
	case 1:	//mon
        strncpy(lockrec.mon_lock,newstr,5);
		break;
    case 2: //tue
        strncpy(lockrec.tue_lock,newstr,5);
        break;
    case 3: //wed
        strncpy(lockrec.wed_lock,newstr,5);
        break;
    case 4: //thu
        strncpy(lockrec.thu_lock,newstr,5);
        break;
    case 5: //fri
        strncpy(lockrec.fri_lock,newstr,5);
        break;
    case 6: //sat
        strncpy(lockrec.sat_lock,newstr,5);
        break;
	}
}
else
{
    switch(dayindex)
    {
    case 0: //sun
        strncpy(lockrec.sun_unlock,newstr,5);
        break;
    case 1: //mon
        strncpy(lockrec.mon_unlock,newstr,5);
        break;
    case 2: //tue
        strncpy(lockrec.tue_unlock,newstr,5);
        break;
    case 3: //wed
        strncpy(lockrec.wed_unlock,newstr,5);
        break;
    case 4: //thu
        strncpy(lockrec.thu_unlock,newstr,5);
        break;
    case 5: //fri
        strncpy(lockrec.fri_unlock,newstr,5);
        break;
    case 6: //sat
        strncpy(lockrec.sat_unlock,newstr,5);
        break;
    }

}





	SetLockTimerLabels(selected_lock);
}



/*
	write the lockrec struct back to the DBF:locks
*/

void WriteLockTimes(void)
{
	char buffer[500];
	string query;

	query="UPDATE locks SET sun_lock='" + (string) lockrec.sun_lock + "',sun_unlock='"+ (string)lockrec.sun_unlock+"'";
	query +=",mon_lock='"+ (string) lockrec.mon_lock + "',mon_unlock='"+ (string) lockrec.mon_unlock + "'";
    query +=",tue_lock='"+ (string) lockrec.tue_lock + "',tue_unlock='"+ (string) lockrec.tue_unlock + "'";
    query +=",wed_lock='"+ (string) lockrec.wed_lock + "',wed_unlock='"+ (string) lockrec.wed_unlock + "'";
    query +=",thu_lock='"+ (string) lockrec.thu_lock + "',thu_unlock='"+ (string) lockrec.thu_unlock + "'";
    query +=",fri_lock='"+ (string) lockrec.fri_lock + "',fri_unlock='"+ (string) lockrec.fri_unlock + "'";
    query +=",sat_lock='"+ (string) lockrec.sat_lock + "',sat_unlock='"+ (string) lockrec.sat_unlock + "' ";

	// now add in the day enables
	query +=",sun_enable='"+  bool_to_string(lockrec.sun_enable)  +"' ";
    query +=",mon_enable='"+  bool_to_string(lockrec.mon_enable) +"' ";
    query +=",tue_enable='"+  bool_to_string(lockrec.tue_enable) +"' ";
    query +=",wed_enable='"+  bool_to_string(lockrec.wed_enable) +"' ";
    query +=",thu_enable='"+  bool_to_string(lockrec.thu_enable) +"' ";
    query +=",fri_enable='"+  bool_to_string(lockrec.fri_enable) +"' ";
    query +=",sat_enable='"+  bool_to_string(lockrec.sat_enable) +"' ";

	query +=",alldayssame='"+ bool_to_string(lockrec.alldayssame) +"' ";

	query +="WHERE lockname='" + (string) lockrec.lockname +"';";

	printf("QUERY: %s\n",query.c_str() );

	// returns 0 on success, 1 on error
	sprintf(buffer,"%s",query.c_str() );
    int result =  QueryDBF(&localDBF,buffer );
	if (result)
	{
		string msg=getMessage(120,FALSE);
		sprintf(gen_buffer,"%s",msg.c_str());
		WriteSystemLog(gen_buffer);
	}

}


/*
	fn is used when setting all days the same for lock/unlock times
*/

void SetLockUnlockTimes(string locktime, string unlocktime)
{

    strncpy(lockrec.sun_lock,locktime.c_str(),5);
    strncpy(lockrec.sun_unlock,unlocktime.c_str(),5);
	strncpy(lockrec.mon_lock,locktime.c_str(),5);
    strncpy(lockrec.mon_unlock,unlocktime.c_str(),5);
    strncpy(lockrec.tue_lock,locktime.c_str(),5);
    strncpy(lockrec.tue_unlock,unlocktime.c_str(),5);
    strncpy(lockrec.wed_lock,locktime.c_str(),5);
    strncpy(lockrec.wed_unlock,unlocktime.c_str(),5);
    strncpy(lockrec.thu_lock,locktime.c_str(),5);
    strncpy(lockrec.thu_unlock,unlocktime.c_str(),5);
    strncpy(lockrec.fri_lock,locktime.c_str(),5);
    strncpy(lockrec.fri_unlock,unlocktime.c_str(),5);
    strncpy(lockrec.sat_lock,locktime.c_str(),5);
    strncpy(lockrec.sat_unlock,unlocktime.c_str(),5);


}



/*
	get the lock and unlock times from the DBF
	for the currently selected lock
	and populate the lockrec struct
*/
void ReadLockTimes(void)
{
	char query[200];
    sprintf(query,"SELECT *  FROM locks WHERE lockname='%s' AND enabled='1';",selected_lock);
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer

	bzero(lockrec.lockname,11);
	bzero(lockrec.sun_lock,6);
    bzero(lockrec.sun_unlock,6);
    bzero(lockrec.mon_lock,6);
    bzero(lockrec.mon_unlock,6);
    bzero(lockrec.tue_lock,6);
    bzero(lockrec.tue_unlock,6);
    bzero(lockrec.wed_lock,6);
    bzero(lockrec.wed_unlock,6);
    bzero(lockrec.thu_lock,6);
    bzero(lockrec.thu_unlock,6);
    bzero(lockrec.fri_lock,6);
    bzero(lockrec.fri_unlock,6);
    bzero(lockrec.sat_lock,6);
    bzero(lockrec.sat_unlock,6);


	strncpy(lockrec.lockname,localDBF.row[0],10);
	strncpy(lockrec.sun_lock,localDBF.row[1],5);
    strncpy(lockrec.sun_unlock,localDBF.row[2],5);
    strncpy(lockrec.mon_lock,localDBF.row[3],5);
    strncpy(lockrec.mon_unlock,localDBF.row[4],5);
    strncpy(lockrec.tue_lock,localDBF.row[5],5);
    strncpy(lockrec.tue_unlock,localDBF.row[6],5);
    strncpy(lockrec.wed_lock,localDBF.row[7],5);
    strncpy(lockrec.wed_unlock,localDBF.row[8],5);
    strncpy(lockrec.thu_lock,localDBF.row[9],5);
    strncpy(lockrec.thu_unlock,localDBF.row[10],5);
    strncpy(lockrec.fri_lock,localDBF.row[11],5);
    strncpy(lockrec.fri_unlock,localDBF.row[12],5);
    strncpy(lockrec.sat_lock,localDBF.row[13],5);
    strncpy(lockrec.sat_unlock,localDBF.row[14],5);

	// lock enable/disabled
	lockrec.lock_enable= atoi(localDBF.row[15]);

	// get active status for each day
	lockrec.sun_enable=atoi(localDBF.row[16]);
    lockrec.mon_enable=atoi(localDBF.row[17]);
    lockrec.tue_enable=atoi(localDBF.row[18]);
    lockrec.wed_enable=atoi(localDBF.row[19]);
    lockrec.thu_enable=atoi(localDBF.row[20]);
    lockrec.fri_enable=atoi(localDBF.row[21]);
    lockrec.sat_enable=atoi(localDBF.row[22]);

	lockrec.alldayssame= atoi(localDBF.row[23]);

}



/*
	set the HR and MIN dropdowns to show
	the currently used value from the DBF

	set lockhr,lockmin,unlockhr,unlockmin comboboxes to the appropriate values
*/
void SetTimeCombos(void)
{
	string msg;
	gint index;
	char * p;
	char tmp[5];

//printf("SetTimeCombos()\n");
	msg=getMessage(51,FALSE);	// Sun
    if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.sun_lock;
    msg=getMessage(52,FALSE);	//Mon
 	if (strcmp(selected_day,msg.c_str() )==0)	p=lockrec.mon_lock;

    msg=getMessage(53,FALSE);	// Tue
 	if (strcmp(selected_day,msg.c_str() )==0)	p=lockrec.tue_lock;

    msg=getMessage(54,FALSE);	// Wed
     if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.wed_lock;

    msg=getMessage(55,FALSE);	// Thu
    if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.thu_lock;

    msg=getMessage(56,FALSE);	// Fri
    if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.fri_lock;

    msg=getMessage(57,FALSE);	// Sat
     if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.sat_lock;


printf("\n\n");
printf("selected_day:: %s\n",selected_day);

	bzero(tmp,5);
	strncpy(tmp,p,2);
	index = atoi(tmp);
printf("LOCK HOUR INDEX: %d   tmp:%s\n",index,tmp);
	gtk_combo_box_set_active(app_ptr->lock_hr,index);

    bzero(tmp,5);
	strncpy(tmp,p+3,2);
	index=atoi(tmp)/5;
printf("LOCK MIN INDEX: %d   tmp:%s\n",index,tmp);
	gtk_combo_box_set_active(app_ptr->lock_min,index);


    msg=getMessage(51,FALSE);   // Sun
    if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.sun_unlock;
    msg=getMessage(52,FALSE);   //Mon
    if (strcmp(selected_day,msg.c_str() )==0)   p=lockrec.mon_unlock;

    msg=getMessage(53,FALSE);   // Tue
    if (strcmp(selected_day,msg.c_str() )==0)   p=lockrec.tue_unlock;

    msg=getMessage(54,FALSE);   // Wed
     if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.wed_unlock;

    msg=getMessage(55,FALSE);   // Thu
    if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.thu_unlock;

    msg=getMessage(56,FALSE);   // Fri
    if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.fri_unlock;

    msg=getMessage(57,FALSE);   // Sat
     if (strcmp(selected_day,msg.c_str() )==0)  p=lockrec.sat_unlock;


    bzero(tmp,5);
    strncpy(tmp,p,2);
    index = atoi(tmp);
	printf("UNLOCK HOUR INDEX: %d\n",index);
	gtk_combo_box_set_active(app_ptr->unlock_hr,index);

    bzero(tmp,5);
    strncpy(tmp,p+3,2);
    index=atoi(tmp)/5;
	printf("UNLOCK min INDEX: %d   tmp:%s\n",index,tmp);
	gtk_combo_box_set_active(app_ptr->unlock_min,index);

printf("\n\n");
}



/*
	set the timelock values on the lock configuration screen
	from the database


*/

void SetLockTimerLabels(char * lockname)
{
	// use lockrec data and fill in the time labels for each day

	gtk_label_set_label(GTK_LABEL(app_ptr->sun_lock_lbl),lockrec.sun_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->sun_unlock_lbl),lockrec.sun_unlock);

    gtk_label_set_label(GTK_LABEL(app_ptr->mon_lock_lbl),lockrec.mon_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->mon_unlock_lbl),lockrec.mon_unlock);

    gtk_label_set_label(GTK_LABEL(app_ptr->tue_lock_lbl),lockrec.tue_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->tue_unlock_lbl),lockrec.tue_unlock);

    gtk_label_set_label(GTK_LABEL(app_ptr->wed_lock_lbl),lockrec.wed_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->wed_unlock_lbl),lockrec.wed_unlock);

    gtk_label_set_label(GTK_LABEL(app_ptr->thu_lock_lbl),lockrec.thu_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->thu_unlock_lbl),lockrec.thu_unlock);

    gtk_label_set_label(GTK_LABEL(app_ptr->fri_lock_lbl),lockrec.fri_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->fri_unlock_lbl),lockrec.fri_unlock);

    gtk_label_set_label(GTK_LABEL(app_ptr->sat_lock_lbl),lockrec.sat_lock);
    gtk_label_set_label(GTK_LABEL(app_ptr->sat_unlock_lbl),lockrec.sat_unlock);

}


extern "C" void unlock_min_changed_cb( GtkComboBox *widget, gpointer user_data)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->unlock_min));
    StoreLockTime(item_text, FALSE,FALSE);
    g_free(item_text);
}

extern "C" void unlock_hr_changed_cb( GtkComboBox *widget, gpointer user_data)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->unlock_hr));
    StoreLockTime(item_text, FALSE,TRUE);
    g_free(item_text);
}



extern "C" void lock_min_changed_cb( GtkComboBox *widget, gpointer user_data)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->lock_min));
    StoreLockTime(item_text, TRUE,FALSE);
    g_free(item_text);
}




extern "C" void lock_hr_changed_cb( GtkComboBox *widget, gpointer user_data)
{
	gchar *item_text = 0;	// selected item text from text combo box
//	gint item_num;
//	item_num = gtk_combo_box_get_active(GTK_COMBO_BOX(app_ptr->lock_hr));

	// get selected item text from GtkComboBoxText object
	item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->lock_hr));
	if (item_text == NULL)
	{
	}
	StoreLockTime(item_text, TRUE, TRUE);
	g_free(item_text);
}


extern "C" void DaySelector_changed_cb( GtkComboBox *widget, gpointer user_data)
{
    gchar *item_text = 0;   // selected item text from text combo box

	changing_day=TRUE;	// set this flag so that StoreLockTime() will do nothing, because
						// changing day causes the hr/min combos to be written to, triggering StoreLockTime()


    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->DaySelector));
	strcpy(selected_day,item_text);

	g_free(item_text);
	SetLockTimerLabels(selected_lock);		// set the times for each day
	SetTimeCombos();						// highlight current selection
	printf("DAY: %s\n",selected_day);

	changing_day=FALSE;		// re-enable StoreLockTime()
}


/*
	user has selected a different lock, update the displayed lock/unlock times
*/
extern "C" void LockSelector_changed_cb( GtkComboBox *widget, gpointer user_data)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->LockSelector));



	if (strcmp(item_text,lock_menu.outter.c_str() )==0)
		strcpy(selected_lock,"outterdoor");

    if (strcmp(item_text,lock_menu.inner.c_str() )==0)
        strcpy(selected_lock,"innerdoor");

    if (strcmp(item_text,lock_menu.shutter.c_str() )==0)
        strcpy(selected_lock,"shutter");

    if (strcmp(item_text,lock_menu.sidecar.c_str() )==0)
        strcpy(selected_lock,"sidecar");

    if (strcmp(item_text,lock_menu.base.c_str() )==0)
        strcpy(selected_lock,"base");

//  strcpy(selected_lock,item_text);
    printf("selected lock: %s",selected_lock);


	SetTimeCombos();
    SetLockTimerLabels( selected_lock);

    g_free(item_text);


}



uint32_t inv_handle;


extern "C"
{

// update the showload() window inventory parms

static void UpdateUTDInventory(int nodeid)
{
	char buff[10];
    int * inv = GetUTDInventory();	// get inv from d8c driver

for (int n=0;n<8;n++)
	printf("Inv%d: %d\n",n,inv[n]);

//printf("test\n");
	sprintf(buff,"%d",(int) inv[0]);
	gtk_label_set_label( GTK_LABEL(app_ptr->inv0),buff );

    sprintf(buff,"%d",inv[1]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv1),buff );

    sprintf(buff,"%d",inv[2]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv2),buff );

    sprintf(buff,"%d",inv[3]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv3),buff );

    sprintf(buff,"%d",inv[4]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv4),buff );

    sprintf(buff,"%d",inv[5]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv5),buff );

    sprintf(buff,"%d",inv[6]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv6),buff );

    sprintf(buff,"%d",inv[7]);
    gtk_label_set_label( GTK_LABEL(app_ptr->inv7),buff );


}

} //extern


// show the LOAD UTD window
void ShowLoad(void)
{
    string msg;
    gtk_widget_show(app_ptr->load_window);

	inv_handle=AddCallBack(&UpdateUTDInventory);	// add to our callback quque

	int * inv=GetUTDInventory();
printf("ShowLoad Inventory\n");
for (int x=0;x<8;x++)
	printf("col %d: %d\n",x,inv[x]);

}



/*
extern "C" bool load_button_clicked_cb( GtkButton *button, AppWidgets *app)
{
	Enable_Load_D8C();
}

extern "C" bool stop_load_button_clicked_cb( GtkButton *button, AppWidgets *app)
{
	Disable_Load_D8C();
}

extern "C" bool close_load_button_clicked_cb( GtkButton *button, AppWidgets *app)
{
    Disable_Load_D8C();
	RemoveCallBack(inv_handle);	// remove our callback
    gtk_widget_hide(app_ptr->load_window);
}


extern "C" bool unload_all_button_clicked_cb( GtkButton *button, AppWidgets *app)
{
printf("UNLOAD ALL BUTTON\n");
	Unload_D8C(8);
}
*/


// LOGIN WINDOW
void ShowLogin(void)
{
    gtk_widget_show(app_ptr->login_window);

}

extern "C" bool on_one_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_two_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_three_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_four_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_five_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_six_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_seven_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_eight_btn_clicked( GtkButton *button, AppWidgets *app)
{
}


extern "C" bool on_nine_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_zero_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_tab_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_bksp_btn_clicked( GtkButton *button, AppWidgets *app)
{
}




extern "C" bool on_en_btn_clicked( GtkButton *button, AppWidgets *app)
{
	printf("ENGLISH btn\n");
}

extern "C" bool on_es_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("SPANISH btn\n");
}

extern "C" bool on_fr_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("FRENCH btn\n");
}

extern "C" bool on_he_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("NEBREW btn\n");
}

extern "C" bool on_login_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("LOGIN btn\n");
    gtk_widget_hide(app_ptr->login_window);
	ShowMainWindow();
}



extern "C" bool on_login_exit_btn_clicked( GtkButton *button, AppWidgets *app)
{
//    gtk_widget_hide(app_ptr->login_window);
	gtk_window_close(GTK_WINDOW(app_ptr->login_window));
}



// MAIN WINDOW
void ShowMainWindow(void)
{
    gtk_widget_show_all(app_ptr->main_screen);
}



// MAINTENANCE WINDOW

void ShowMaint(void)
{
    gtk_widget_show(app_ptr->maint_window);
}


extern "C" bool on_mei_button_clicked( GtkButton *button, AppWidgets *app)
{
printf("Maint:MEI BUTTON\n");
	ShowMEIMaint();
}

extern "C" bool on_utd_button_clicked( GtkButton *button, AppWidgets *app)
{
printf("Maint:UTD BUTTON\n");
	ShowUTDMaint();
}


extern "C" bool on_main_lock_button_clicked( GtkButton *button, AppWidgets *app)
{
printf("Maint:LOCK BUTTON\n");
}



extern "C" bool on_maint_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->maint_window);
}


// MEI maintenance window

void ShowMEIMaint(void)
{
    gtk_widget_show(app_ptr->mei_maint_window);
}

extern "C" bool on_mei_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->mei_maint_window);
}

extern "C" bool on_mei_reset_btn_clicked( GtkButton *button, AppWidgets *app)
{

}




// UTD maintenance window

void ShowUTDMaint(void)
{
    gtk_widget_show(app_ptr->utd_maint_window);
}

extern "C" bool on_utd_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->utd_maint_window);
}

extern "C" bool on_zero_utd_btn_clicked( GtkButton *button, AppWidgets *app)
{
}


extern "C" bool on_utd_set_addr_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_utd_get_addr_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_utd_get_inventory_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_utd_unloadall_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_utd_reset_btn_clicked( GtkButton *button, AppWidgets *app)
{
}




// show the lock config window
void ShowConfig(void)
{
	string msg;
	gtk_widget_show(app_ptr->lockconfig_window);

	msg=getMessage(51,FALSE);	// Sun
	strcpy(selected_day,msg.c_str() );

	msg = getMessage(101,FALSE);
	gtk_window_set_title( GTK_WINDOW(app_ptr->lockconfig_window), msg.c_str() );

	msg = getMessage(50,FALSE);	// "Close"
	gtk_button_set_label( GTK_BUTTON(app_ptr->lockconfig_close_btn),msg.c_str() );

	msg=getMessage(74,FALSE);	// Time Lock
	gtk_label_set_label(GTK_LABEL(app_ptr->config_timelock),msg.c_str());

    msg=getMessage(70,FALSE);   // LOCK
    gtk_label_set_label(GTK_LABEL(app_ptr->config_lock_word),msg.c_str());
    gtk_label_set_label(GTK_LABEL(app_ptr->config_lock_word_2),msg.c_str());

    msg=getMessage(71,FALSE);   // UNLOCK
    gtk_label_set_label(GTK_LABEL(app_ptr->config_unlock_word),msg.c_str());
    gtk_label_set_label(GTK_LABEL(app_ptr->config_unlock_word_2),msg.c_str());

    msg=getMessage(73,FALSE);   // All Days Same
    gtk_button_set_label(GTK_BUTTON(app_ptr->dayssame),msg.c_str());

    msg=getMessage(75,FALSE);   // Active
    gtk_label_set_label(GTK_LABEL(app_ptr->config_active_lbl),msg.c_str());



	SetConfigLocks();		// fill in lockselector combo and set database
    ReadLockTimes();        // get lock/unlock times from database
	SetTimeCombos();
    SetLockTimerLabels( selected_lock);
	SetConfigActiveBtns();

}

string temp;

/*
	read the HR dropdown on the lock config screen
	and return that string
*/

string GetLockHour(void)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->lock_hr));
    if (item_text == NULL)
    {
    }
	temp =  (string) item_text;
    g_free(item_text);
	return temp;

}


/*
	read the lock MIN dropdown on the lock config screen
	and return that string
*/
string GetLockMin(void)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->lock_min));
	temp = (string) item_text;
    g_free(item_text);
	return temp;
}


/*
	read the unlock HR dropdown on the lock config screen
	and return that string
*/

string GetUNLockHour(void)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->unlock_hr));
    if (item_text == NULL)
    {
    }
    temp =  (string) item_text;
    g_free(item_text);
    return temp;

}


/*
	read the unlock MIN dropdown on the lock config screen
	and return that string
*/

string GetUNLockMin(void)
{
    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->unlock_min));
    temp = (string) item_text;
    g_free(item_text);
    return temp;
}



/*
	set all the active buttons with the setting stored in the DBF, which is
	now in lockrec
*/
void SetConfigActiveBtns(void)
{

	gtk_toggle_button_set_active(app_ptr->sun_active,lockrec.sun_enable);
    gtk_toggle_button_set_active(app_ptr->mon_active,lockrec.mon_enable);
    gtk_toggle_button_set_active(app_ptr->tue_active,lockrec.tue_enable);
    gtk_toggle_button_set_active(app_ptr->wed_active,lockrec.wed_enable);
    gtk_toggle_button_set_active(app_ptr->thu_active,lockrec.thu_enable);
    gtk_toggle_button_set_active(app_ptr->fri_active,lockrec.fri_enable);
    gtk_toggle_button_set_active(app_ptr->sat_active,lockrec.sat_enable);

   gtk_toggle_button_set_active (app_ptr->dayssame, lockrec.alldayssame );
//printf("SetConfigActiveBnts\n");
//printf("sun:%d, mon:%d\n",lockrec.sun_enable,lockrec.mon_enable);

}


// sun ACTIVE button clicked

extern "C" bool sun_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
	if (lockrec.sun_enable)
		lockrec.sun_enable=FALSE;
	else
		lockrec.sun_enable=TRUE;
}

extern "C" bool mon_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
    if (lockrec.mon_enable)
        lockrec.mon_enable=FALSE;
    else
        lockrec.mon_enable=TRUE;
}

extern "C" bool tue_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
    if (lockrec.tue_enable)
        lockrec.tue_enable=FALSE;
    else
        lockrec.tue_enable=TRUE;
}

extern "C" bool wed_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
    if (lockrec.wed_enable)
        lockrec.wed_enable=FALSE;
    else
        lockrec.wed_enable=TRUE;
}

extern "C" bool thu_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
    if (lockrec.thu_enable)
        lockrec.thu_enable=FALSE;
    else
        lockrec.thu_enable=TRUE;
}

extern "C" bool fri_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
    if (lockrec.fri_enable)
        lockrec.fri_enable=FALSE;
    else
        lockrec.fri_enable=TRUE;
}


extern "C" bool sat_active_clicked_cb( GtkButton *button, AppWidgets *app)
{
    if (lockrec.sat_enable)
        lockrec.sat_enable=FALSE;
    else
        lockrec.sat_enable=TRUE;
}



/*
	get the status of the ALL DAYS SAME checkbox
	and set the lock/unlock times for every day to those values

*/


extern "C" bool dayssame_clicked_cb( GtkButton *button, AppWidgets *app)
{
	string tmp;
	printf("days same toggled\n");

	lockrec.alldayssame=gtk_toggle_button_get_active ( app_ptr->dayssame );

	lockhrmin=GetLockHour();
	lockhrmin += ":";
	lockhrmin +=GetLockMin();

	printf("HR:MIN: %s\n",lockhrmin.c_str() );

    unlockhrmin=GetUNLockHour();
    unlockhrmin += ":";
    unlockhrmin +=GetUNLockMin();

    printf("HR:MIN: %s\n",unlockhrmin.c_str() );


	if (lockrec.alldayssame)
		printf("SAME=TRUE\n");
	else
		printf("SAME=FALSE\n");

//printf("setting all lock times to %s, unlock times to %s\n",lockhrmin.c_str(),unlockhrmin.c_str());
	SetLockUnlockTimes(lockhrmin,unlockhrmin);
}

/*
	enable locks in the DBF, per the cfg. vars read from config.xml

*/

void ConfigLocksInDBF(void)
{
    int result;
    string query;

    // disable all locks in database
    sprintf(gen_buffer,"UPDATE locks SET enabled='0';");
    result =  QueryDBF(&localDBF,gen_buffer );


    // check the config variables for enabled locks
    if ( strcmp(cfg.outterlock,"enabled")==0)
    {
        query = "UPDATE locks SET enabled='1' WHERE lockname='outterdoor';";
        // returns 0 on success, nonzero on err
        sprintf(gen_buffer,"%s",query.c_str());
        result =  QueryDBF(&localDBF,gen_buffer );
        if (result !=0)
        {
            sprintf(gen_buffer,"DBF ERROR: %s << failed",query.c_str() );
            WriteSystemLog(gen_buffer);
        }
		else
		{
			sprintf(gen_buffer,"OUTTER_LOCK enabled");
			WriteSystemLog(gen_buffer);
	        Enable_Lock(OUTTER_LOCK);       // in usb_gateway.cpp
		}
    }

    if ( strcmp(cfg.innerlock,"enabled")==0)
    {
        query = "UPDATE locks SET enabled='1' WHERE lockname='innerdoor';";
        sprintf(gen_buffer,"%s",query.c_str());
        result =  QueryDBF(&localDBF,gen_buffer );
        if (result !=0)
        {
            sprintf(gen_buffer,"DBF ERROR: %s << failed",query.c_str() );
            WriteSystemLog(gen_buffer);
        }
        else
        {
            sprintf(gen_buffer,"INNER_LOCK enabled");
            WriteSystemLog(gen_buffer);
            Enable_Lock(INNER_LOCK);       // in usb_gateway.cpp
        }
    }

    if ( strcmp(cfg.shutterlock,"enabled")==0)
    {
        query = "UPDATE locks SET enabled='1' WHERE lockname='shutter';";
        sprintf(gen_buffer,"%s",query.c_str());
        result =  QueryDBF(&localDBF,gen_buffer );
        if (result !=0)
        {
            sprintf(gen_buffer,"DBF ERROR: %s << failed",query.c_str() );
            WriteSystemLog(gen_buffer);
        }
        else
        {
            sprintf(gen_buffer,"SHUTTER_LOCK enabled");
            WriteSystemLog(gen_buffer);
            Enable_Lock(SHUTTER_LOCK);       // in usb_gateway.cpp
        }
    }


    if ( strcmp(cfg.sidecarlock,"enabled")==0)
    {
        query = "UPDATE locks SET enabled='1' WHERE lockname='sidecar';";
        sprintf(gen_buffer,"%s",query.c_str());
        result =  QueryDBF(&localDBF,gen_buffer );
        if (result !=0)
        {
            sprintf(gen_buffer,"DBF ERROR: %s << failed",query.c_str() );
            WriteSystemLog(gen_buffer);
        }
        else
        {
            sprintf(gen_buffer,"SIDECAR_LOCK enabled");
            WriteSystemLog(gen_buffer);
            Enable_Lock(SIDECAR_LOCK);       // in usb_gateway.cpp
        }
    }

    if ( strcmp(cfg.baselock,"enabled")==0)
    {
        query = "UPDATE locks SET enabled='1' WHERE lockname='base'";
        sprintf(gen_buffer,"%s",query.c_str());
        result =  QueryDBF(&localDBF,gen_buffer );
        if (result !=0)
        {
            sprintf(gen_buffer,"DBF ERROR: %s << failed",query.c_str() );
            WriteSystemLog(gen_buffer);
        }
        else
        {
            sprintf(gen_buffer,"BASE_LOCK enabled");
            WriteSystemLog(gen_buffer);
            Enable_Lock(BASE_LOCK);       // in usb_gateway.cpp
        }
    }

}






/*
	populate LockSelector combobox using the configuration vars from fking.xml
	as below. each of these will have "enabled" or "disabled"

	cfg.outterlock
	cfg.innerlock
	cfg.shutterlock
	cfg.sidecarlock
	cfg.baselock

*/


void SetConfigLocks(void)
{
	lock_menu.outter = getMessage(60,FALSE);	// Outter Door
    lock_menu.inner = getMessage(61,FALSE); 	// Inner Door
	lock_menu.shutter = getMessage(62,FALSE); 	// Shutter
	lock_menu.sidecar = getMessage(63,FALSE); 	// Side Car
	lock_menu.base = getMessage(64,FALSE); 		// Base Lock






//	GList *glist=NULL;

	// remove all items from list
//	gtk_combo_box_text_remove_all (app_ptr->LockSelector);

// NOTE: commenting out the below line and hard coding Main Door into GLADE
// is a workaround because the gtk_combo_box_set_active() function wouldn't
// work if LockSelector is defined as a GTK_COMBO_BOX_TEXT object
//	if (cfg.outterlock)
//        gtk_combo_box_text_append_text( app_ptr->LockSelector,outter);

	if (strcmp(cfg.innerlock,"enabled")==0)
        gtk_combo_box_text_append_text( app_ptr->LockSelector,lock_menu.inner.c_str() );

	if (strcmp(cfg.shutterlock,"enabled")==0)
        gtk_combo_box_text_append_text( app_ptr->LockSelector,lock_menu.shutter.c_str() );

	if (strcmp(cfg.sidecarlock,"enabled")==0)
        gtk_combo_box_text_append_text( app_ptr->LockSelector,lock_menu.sidecar.c_str() );

	if(strcmp(cfg.baselock,"enabled")==0)
        gtk_combo_box_text_append_text( app_ptr->LockSelector,lock_menu.base.c_str() );

/*

		glist = g_list_append(glist,outter);
		glist = g_list_append(glist,inner);
		glist = g_list_append(glist,shutter);
		glist = g_list_append(glist,sidecar);
		glist = g_list_append(glist,base);
*/

//	gint index=0;
//    gtk_combo_box_set_active( app_ptr->LockSelector,index);


	// now populate the combobox onscreen
//	gtk_combo_set_popdown_strings ( app_ptr->LockSelector, glist);

}



// hide the lock config window
extern "C" bool on_lockconfig_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
	WriteLockTimes();
	gtk_widget_hide(app_ptr->lockconfig_window);
//	gtk_widget_show_all(app_ptr->main_screen);
}


// LOCKCONFIG SECTION - END
//=============================================================================================



// TIMELOCK SECTION - START
//=============================================================================================

/*
struct timelock
{
	int active=FALSE;
	int lock_index;		// index into locks[] struct for usb_gateway
	int unlock_day;
	int unlock_hour;	// must be in mil format
	int unlock_minute;
	int lock_day;
	int lock_hour;		// must be mil format
	int lock_minute;
	bool locked=TRUE;
	int etime;			// lock delay for this lock
	int seconds;		// used to keep track of how long the lock has been locked, so we know when to send another UNLOCK cmd, so it
						// doesn't automatically re-lock
};
*/

void CheckTL(timelock tl,int index);


int current_hour;	// 0-23
int current_minute;	// 0-59
int curernt_day;	// // [0,6] (Sunday = 0)


void GetCurrentTime(void)
{
	time_t current;
    struct tm * timeptr;
    current = time(NULL);
    timeptr = localtime(&current);

	current_day=timeptr->tm_wday;	// [0,6] (Sunday = 0)
	current_hour=timeptr->tm_hour;	// 0-23
	current_minute=timeptr->tm_min; // 0-59
}



/*
     locks

    1. shutter
    2. main vault
    3. Side car
    4. base
*/

/*
timelock MAIN_VAULT_TL;
timelock SHUTTER_TL;
timelock SIDE_CAR_TL;
timelock BASE_LOCK_TL;
*/

/*
timelock OUTTER_LOCK_TL;
timelock INNER_LOCK_TL;
timelock SHUTTER_LOCK_TL;
timelock SIDECAR_LOCK_TL;
timelock BASE_LOCK_TL;
*/

/*
	this function gets called once per second
*/

void CheckTimeLocks(void)
{
	CheckTL(OUTTER_LOCK_TL,OUTTER_LOCK);
	CheckTL(INNER_LOCK_TL,INNER_LOCK);
	CheckTL(SHUTTER_LOCK_TL,SHUTTER_LOCK);
	CheckTL(SIDECAR_LOCK_TL,SIDECAR_LOCK);
	CheckTL(BASE_LOCK_TL,BASE_LOCK);
}


/*
	check all timelocks to see if action is needed
	this is called once per second
*/

void CheckTL(timelock tl,int lock_index)
{
	if (!tl.active) return;
	tl.seconds++;

	// if the lock delay window is about to close, issue another UNLOCK command to prevent the lock from automatically re-locking
	if (tl.seconds >= tl.etime-3)
	{
		tl.seconds=0;		// reset our counter
		Unlock_Lock(lock_index);
		printf("Unlocking lock at %d sec,  before delay relocks it\n", tl.seconds);
	}

	GetCurrentTime();

/*
	if (!tl.locked)
	{
		// we are UNLOCKED, must make sure to resend UNLOCK command before the lock automatically relocks
		// and repeat until RE-LOCK time arrives
	}
*/

	// test for start of RE-LOCK time
	if (tl.lock_hour == current_hour && tl.lock_minute== current_minute && tl.lock_day==current_day)
	{
		if (!GetIsLocked(lock_index) )
		{
			// lock it
			if (Lock_Lock(lock_index) )
				tl.locked=TRUE;

			else
				tl.locked=FALSE;
		}
	}


	// test for start of UNLOCK window
	if (tl.unlock_hour == current_hour && tl.unlock_minute == current_minute && tl.unlock_day == current_day)
	{
		if (GetIsLocked(lock_index) )
		{
			//TODO we can unlock for a max of 99 seconds at a time. so we will need to monitor and re-issue an unlock command
			// before timer expiration
			//unlock it
			if (Unlock_Lock(lock_index) )	// unlock the lock for .delay value
				tl.locked=TRUE;
			else
				tl.locked=FALSE;
		}
	}
}



// this function is not used

void SetTimeLock(timelock tl, int lock_index,int lock_hour,int lock_min,int lock_day,int unlock_hour,int unlock_min,int unlock_day)
{
	tl.active=TRUE;
	tl.lock_index=lock_index;
	tl.lock_hour=lock_hour;
	tl.lock_minute=lock_min;
	tl.lock_day=lock_day;
	tl.unlock_hour=unlock_hour;
	tl.unlock_minute=unlock_min;
	tl.unlock_day=unlock_day;
	tl.locked=GetIsLocked(lock_index);
	tl.etime=GetLockDelay(lock_index);
}



void timer1_event(void);
void timer2_event(void);
void start_timer1(int seconds);
void start_timer2(int seconds);
void CheckTimeLocks(void);

bool timer1_enabled=FALSE;
bool timer2_enabled=FALSE;
int timer1=0;        // seconds
int timer2=0;
int timer1_expiration;	// in seconds, timer1 will count up to this value and then call timer1_event()
int timer2_expiration;  // in seconds, timer1 will count up to this value and then call timer1_event()


// TIMELOCK SECTION - END
//=============================================================================================



/*
	this is our callback from gtk_nain()
	we set it to happen every TIME_SLICE (100ms_

*/


unsigned int tseconds=0;		// globally available seconds counter

gint time_slice(gpointer data)
{
	static unsigned int counter=0;
	static unsigned int seconds=0;
	static unsigned int lastsecond=0;

	if (!time_slice_enabled) return TRUE;	// if time slice is disabled, return


	counter++;

	// math predicated upon 100ms slices
	// to create a one second timer
	if ((counter % 10)== 0)
	{
		seconds++;
		if (timer1_enabled)		timer1++;
		if (timer2_enabled)		timer2++;
//		printf ("Time Slice: %ds\n",seconds);
	}


	if (timer1 >= timer1_expiration)
		timer1_event();

    if (timer2 >= timer2_expiration)
        timer2_event();


	// CALLBACKS WILL BE RUN HERE
	// one second event timer
	if (lastsecond != seconds)
	{
		ExecCallBacks();		// execute any callbacks in the queue
	}


	// check timelock status every minute
	// the second test is to ensure we only fire once per minute
	if ( ((seconds % 60)==0) && lastsecond != seconds)
	{
		lastsecond=seconds;
		CheckTimeLocks();
        printf ("Time Slice: ONE MINUTE\n");
	}



	tseconds=seconds;
	return TRUE;
}






void start_timer1(int seconds)
{
    timer1=0;
    timer1_expiration=seconds;
    timer1_enabled=TRUE;

}




void start_timer2(int seconds)
{
	timer2=0;
	timer2_expiration=seconds;
	timer2_enabled=TRUE;
}





/*
	timer1 has expired, handle the event

*/
void timer1_event(void)
{
	timer1_enabled=FALSE;
	timer1=0;
}


/*
    timer2 has expired, handle the event

*/

void timer2_event(void)
{
	timer2_enabled=FALSE;
	timer2=0;
}



/*
	ok, main() is dead and stinkin, the rest of this module contains functions needed by the GTK/GDK GUI
	as well as time_slice() and various other logistics functions required

	NOTE: all callbacks intended on interfacing with the GTK/GDK GUI, must be declared as [extern "C"]
	so that the linker will align the pointers correctly between the GTK/GDK std C modules and C++ modules.
	This proved a rather difficult task to incorporate, however, the result is small size and faster run time speeds
	and more "green" usage of the computer resources such as HD, RAM, and most of all, extremely efficient usage of
	CPU cycles.

		Teal 								Gray
		100%	(0,146,159)					(65,83,93)
		80%		(0,161,174)					(100,116,127)
		60%		(94,179,190)				(132,146,156)
		40%		(146,201,208)				(167,179,188)
		20%		(96,224,228)				(200,214,220)


*/


void SetColors(void)
{

	// GdkColor values are 0 -> 65535

	float conv = 65535.0/255.0;

	// highlight color after clicking
	high_color.red=236*conv;
	high_color.green= 130*conv;
	high_color.blue=26*conv;

	// hover color, when hovering
	hover_color.red=236*conv;
	hover_color.green= 187*conv;
	hover_color.blue=134*conv;


	// not bad
	flow_color.red=94*conv;
	flow_color.green= 179*conv;
	flow_color.blue=190*conv;

	// not bad
//	flow_color.red=65*conv;
//	flow_color.green=97*conv;
//	flow_color.blue=106*conv;

	// 80%
	flow_color.red=0*conv;
	flow_color.green= 161*conv;
	flow_color.blue=174*conv;

	// 100%
	flow_teal.red=0*conv;
	flow_teal.green= 146*conv;
	flow_teal.blue=159*conv;

	flow_gray.red=65*conv;
	flow_gray.green=97*conv;
	flow_gray.blue=106*conv;

	flow_orange.red = 247*conv;
	flow_orange.green=192*conv;
	flow_orange.blue=103*conv;


	flow_white.red = 250 * conv;
	flow_white.green = 250 * conv;
	flow_white.blue = 250 * conv;

	flow_light_teal.red = 96 * conv;
	flow_light_teal.green = 224 * conv;
	flow_light_teal.blue = 228 * conv;


	// set colors in help window
//gtk_widget_modify_base(app_ptr->help_window,GTK_STATE_NORMAL,&flow_light_teal);
//gtk_widget_modify_bg(app_ptr->help_window, GTK_STATE_NORMAL, &flow_light_teal);




//	flow_color.red=65*conv;
//	flow_color.green=97*conv;
//	flow_color.blue=106*conv;


//gdk_color_parse("white",&flow_color);
//	gtk_widget_modify_base(app_ptr->operation_hbox,GTK_STATE_NORMAL,&flow_color);
//	gtk_widget_modify_bg(app_ptr->operation_hbox, GTK_STATE_NORMAL, &flow_color);
//	gtk_widget_modify_bg(app_ptr->operation_hbox, GTK_STATE_ACTIVE, &flow_color);



}



/*

	set the window sizes  for all windows
	h/v res comes from screen file, as read by GetScreenRes()

*/

void SetScreenSizes(void)
{
	gtk_window_set_default_size(GTK_WINDOW(app_ptr->main_screen), screenres.horiz, screenres.vert);

    gtk_window_set_default_size(GTK_WINDOW(app_ptr->lockconfig_window), screenres.horiz, screenres.vert);
//    gtk_window_set_default_size(GTK_WINDOW(app_ptr->load_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->maint_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->utd_maint_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->login_window), screenres.horiz, screenres.vert);


}




/*
	set all the labels, buttons etc text from the language file
*/

void SetLabels(void)
{
	string msg;




// main window

    msg = getMessage(243,FALSE); // "LOCK"
    gtk_button_set_label( GTK_BUTTON(app_ptr->lock_btn),msg.c_str() );

    msg = getMessage(250,FALSE); // "LOAD"
    gtk_button_set_label( GTK_BUTTON(app_ptr->load_btn),msg.c_str() );

    msg = getMessage(251,FALSE); // "UNLOAD"
    gtk_button_set_label( GTK_BUTTON(app_ptr->unload_btn),msg.c_str() );

    msg = getMessage(252,FALSE); // "MAINT"
    gtk_button_set_label( GTK_BUTTON(app_ptr->maint_btn),msg.c_str() );

    msg = getMessage(253,FALSE); // "LOCK CONFIG"
    gtk_button_set_label( GTK_BUTTON(app_ptr->lock_config_btn),msg.c_str() );


// showconfig window
//    msg = getMessage(102,FALSE);
//    gtk_window_set_title( GTK_WINDOW(app_ptr->load_window), msg.c_str() );

/*
    msg = getMessage(200,FALSE);
// "Load" // gtk_button_set_label( GTK_BUTTON(app_ptr->load_button),msg.c_str() );
    gtk_button_set_label( GTK_BUTTON(app_ptr->load_button),msg.c_str() );

    msg = getMessage(201,FALSE); // "Stop load"
    gtk_button_set_label( GTK_BUTTON(app_ptr->stop_load_button),msg.c_str() );

    msg = getMessage(50,FALSE); // "Close"
    gtk_button_set_label( GTK_BUTTON(app_ptr->close_load_button),msg.c_str() );

    msg = getMessage(202,FALSE); // "Unload ALL"
    gtk_button_set_label( GTK_BUTTON(app_ptr->unload_all_button),msg.c_str() );
*/

// maint_window

    msg = getMessage(240,FALSE);
    gtk_window_set_title( GTK_WINDOW(app_ptr->maint_window), msg.c_str() );

    gtk_label_set_label(GTK_LABEL(app_ptr->maint_title),msg.c_str() );


    msg=getMessage(241,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->mei_button),msg.c_str() );



    msg=getMessage(242,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_button),msg.c_str() );

    msg=getMessage(243,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->maint_lock_button),msg.c_str() );

    msg=getMessage(50,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->maint_close_btn),msg.c_str() );



//utd_window

    msg = getMessage(225,FALSE);
    gtk_window_set_title( GTK_WINDOW(app_ptr->utd_maint_window), msg.c_str() );

    gtk_label_set_label(GTK_LABEL(app_ptr->utd_maint_title),msg.c_str() );

    msg=getMessage(220,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->zero_utd_btn),msg.c_str() );

    msg=getMessage(221,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_set_addr_btn),msg.c_str() );

    msg=getMessage(222,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_get_addr_btn),msg.c_str() );


    msg=getMessage(223,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_get_inventory_btn),msg.c_str() );

    msg=getMessage(224,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_unloadall_btn),msg.c_str() );

    msg=getMessage(226,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_reset_btn),msg.c_str() );

    msg=getMessage(50,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->utd_close_btn),msg.c_str() );


// mei_maint_window

    msg=getMessage(260,FALSE);
    gtk_label_set_label(GTK_LABEL(app_ptr->mei_maint_title),msg.c_str() );

    msg=getMessage(226,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->mei_reset_btn),msg.c_str() );

    msg=getMessage(50,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->mei_close_btn),msg.c_str() );




}


void SetMainScreenFont(void)
{
	int font_size=7000;
	char buffer[80];

		sprintf(buffer,"<span size='%d'</span>",font_size);
//		cell_label = gtk_label_new(NULL);
//		gtk_label_set_markup( GTK_LABEL(app_ptr->textbuffer1), buffer);
//        gtk_label_set_markup( GTK_TEXT_BUFFER(app_ptr->textbuffer1), buffer);

//		gtk_misc_set_alignment( GTK_MISC(cell_label), 0.1,0.0);
//		gtk_table_attach_defaults( GTK_TABLE(app_ptr->screw_comp_table),cell_label,	0,1,  row,(row+1)	);

}


void SetMainScreenTitle(void)
{
    sprintf(gen_buffer,"%s ver %d.%d.%d",product_name,MAJOR_VERSION, MINOR_VERSION, MICRO_VERSION );
    gtk_window_set_title( GTK_WINDOW(app_ptr->main_screen), gen_buffer );

//	gtk_text_buffer_set_text( GTK_TEXT_BUFFER(app_ptr->textbuffer1), msg.c_str(), msg.length() );
//	markup = Markup(HELP[help_window_page].title);
//	gtk_label_set_markup(app_ptr->help_title_label,markup.c_str() );


}


//BUTTONS-MAINSCREEN

extern "C" bool on_escape_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("ESC\n");
}

extern "C" bool on_help_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("HELP\n");
}



/*
extern "C" bool on_one_btn_clicked( GtkButton *button, AppWidgets *app)
{
	printf("ONE\n");
}


extern "C" bool on_two_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("TWO\n");
}

extern "C" bool on_three_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("THREE\n");
}

extern "C" bool on_four_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("FOUR\n");
}


extern "C" bool on_five_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("FIVE\n");
}


extern "C" bool on_six_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("SIX\n");
}


extern "C" bool on_seven_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("SEVEN\n");
}

extern "C" bool on_eight_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("EIGHT\n");
}

extern "C" bool on_nine_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("NINE\n");
}


extern "C" bool on_zero_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("ZERO\n");
}
*/

extern "C" bool on_enter_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("ENTER\n");
}

extern "C" bool on_clear_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("CLEAR\n");
}


extern "C" bool on_drop_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("DROP\n");
}

extern "C" bool on_load_btn1_clicked( GtkButton *button, AppWidgets *app)
{
//	ShowLoad();
    printf("LOAD\n");
}


extern "C" bool on_doors_btn_clicked( GtkButton *button, AppWidgets *app)
{
	printf("calling unlock_lock\n");
	Unlock_Lock(0);		// in usb_gateway.cpp
    printf("DOORS\n");
}


extern "C" bool on_display_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("DISPLAY\n");
}


extern "C" bool on_report_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("REPORT\n");
}

extern "C" bool on_vend_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("VEND\n");
}


extern "C" bool on_unload_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("UNLOAD\n");
}


extern "C" bool on_buy_change_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("BUY CHANGE\n");
}



extern "C" bool on_pgrm_btn_clicked( GtkButton *button, AppWidgets *app)
{
    ShowConfig();
    printf("PGRM\n");
}

extern "C" bool on_drop_set_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("DROP SET\n");
}

extern "C" bool on_insta1_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("INSTA 1\n");
	ShowMaint();

}


extern "C" bool on_insta2_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("INSTA 2\n");
}

extern "C" bool on_insta3_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("INSTA 3\n");
}

extern "C" bool on_insta4_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("INSTA 4\n");
}


extern "C" bool on_left1_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("LEFT 1\n");
}

extern "C" bool on_left2_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("LEFT 2\n");
}
extern "C" bool on_left3_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("LEFT 3\n");
}
extern "C" bool on_left4_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("LEFT 4\n");
}

extern "C" bool on_up_right_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("UP RIGHT\n");
}
extern "C" bool on_dn_left_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("DN LEFT\n");
}




// MAIN MENU
extern "C" bool on_lock_config_btn_clicked( GtkButton *button, AppWidgets *app)
{
    ShowConfig();
    printf("LOCK CONFIG\n");
}


extern "C" bool on_maint_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("MAINT\n");
    ShowMaint();

}

extern "C" bool on_load_btn_clicked( GtkButton *button, AppWidgets *app)
{
    ShowLoad();
    printf("LOAD\n");
}

extern "C" bool on_unload_btn1_clicked( GtkButton *button, AppWidgets *app)
{
    printf("UNLOAD\n");
}


extern "C" bool on_lock_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("calling unlock_lock\n");
    Unlock_Lock(0);     // in usb_gateway.cpp
    printf("DOORS\n");
}




/*
	remove the currently displayed widget from the actuator_configuration_frame so that
	we can reparent the new widget into place
*/


#define REPARENT(old_parent,new_parent,widget) \
{ \
		g_object_ref(widget); /* save a copy of the reference (increase reference count) */ \
		gtk_container_remove( GTK_CONTAINER(old_parent), widget ); /* remove the widget from the old_parent*/ \
		gtk_container_add( GTK_CONTAINER(new_parent), widget); /* add the widget to the new parent */ \
		g_object_unref( widget); /* release the reference */ \
}






/*
	Read the config.xml file and set the local variables as described therein

	RETURNS: TRUE on success, else FALSE
*/



bool ConfigSetup(bool silent)
{



	TiXmlDocument doc(config_file);
	bool loadOkay = doc.LoadFile();
	if (!loadOkay)
	{
		if (!silent)
			printf("Error reading config file %s Error: %s\n",config_file,doc.ErrorDesc());
		return false;
	}



	TiXmlElement *pelem;
	TiXmlElement* elem;
	TiXmlElement* root;

	root = doc.FirstChildElement();
	if(root == NULL)
	{
	    printf( "Failed to load file: No root element.");
	    doc.Clear();
	    return false;
	}

	for(elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		string elemName = elem->Value();

		const char* attr;
		if (elemName == "screen")
		{
			pelem = elem->FirstChildElement("orientation");
			if (pelem)
				strcpy(cfg.screentxt, (char*) pelem->GetText());
			else
				strcpy(cfg.screentxt,  (char*) "landscape");


		}

		if(elemName == "localDBF")
		{
			pelem = elem->FirstChildElement("ip");
//			char * ip = (char*) pelem->Value();
			if (pelem)
				strcpy(cfg.iptxt, (char*) pelem->GetText());
			else
				strcpy(cfg.iptxt, (char*) "localhost");

			pelem = elem->FirstChildElement("user");
			if (pelem)
				strcpy(cfg.usertxt, (char*) pelem->GetText());
			else
				strcpy(cfg.usertxt,  (char*) "fkinguser101");

			pelem = elem->FirstChildElement("password");
			if (pelem)
				strcpy(cfg.pwtxt, (char*) pelem->GetText());
			else
				strcpy(cfg.pwtxt, (char*) "glimpbust345");

			pelem = elem->FirstChildElement("database");
			if (pelem)
				strcpy(cfg.dbftxt, (char*) pelem->GetText());
			else
				strcpy(cfg.dbftxt, (char*) "fking");

			pelem = elem->FirstChildElement("port");
			if (pelem)
				strcpy(cfg.porttxt, (char*) pelem->GetText());
			else
				strcpy(cfg.porttxt, (char*) "3306");


			if (!silent)
				printf("LOCAL:: ip:%s  user:%s  pw:%s dbf:%s  port:%s  \n",cfg.iptxt,cfg.usertxt,cfg.pwtxt,cfg.dbftxt,cfg.porttxt);

			// now populate our local object
			strcpy(localDBF.user,cfg.usertxt);
			strcpy(localDBF.password,cfg.pwtxt);
			strcpy(localDBF.database,cfg.dbftxt);
			localDBF.port= atoi(cfg.porttxt);

		}



		if(elemName == "devices")
		{
            pelem = elem->FirstChildElement("validator1");
            if (pelem)
                strcpy(cfg.validator1, (char*) pelem->GetText());
            else
                strcpy(cfg.validator1, (char*) "disabled");

            pelem = elem->FirstChildElement("validator2");
            if (pelem)
                strcpy(cfg.validator2, (char*) pelem->GetText());
            else
                strcpy(cfg.validator2, (char*) "disabled");

            pelem = elem->FirstChildElement("ucd1");
            if (pelem)
                strcpy(cfg.ucd1, (char*) pelem->GetText());
            else
                strcpy(cfg.ucd1, (char*) "disabled");

            pelem = elem->FirstChildElement("ucd2");
            if (pelem)
                strcpy(cfg.ucd2, (char*) pelem->GetText());
            else
                strcpy(cfg.ucd2, (char*) "disabled");


            pelem = elem->FirstChildElement("utd");
            if (pelem)
                strcpy(cfg.utd, (char*) pelem->GetText());
            else
                strcpy(cfg.utd, (char*) "disabled");



            pelem = elem->FirstChildElement("outterlock");
            if (pelem)
                strcpy(cfg.outterlock,(char*) pelem->GetText());
            else
                strcpy(cfg.outterlock, (char*) "disabled");

            pelem = elem->FirstChildElement("innerlock");
            if (pelem)
                strcpy(cfg.innerlock, (char*) pelem->GetText());
            else
                strcpy(cfg.innerlock, (char*) "disabled");

            pelem = elem->FirstChildElement("shutterlock");
            if (pelem)
                strcpy(cfg.shutterlock, (char*) pelem->GetText());
            else
                strcpy(cfg.shutterlock, (char*) "disabled");

            pelem = elem->FirstChildElement("sidecarlock");
            if (pelem)
                strcpy(cfg.sidecarlock, (char*) pelem->GetText());
            else
                strcpy(cfg.sidecarlock, (char*) "disabled");

            pelem = elem->FirstChildElement("baselock");
            if (pelem)
                strcpy(cfg.baselock, (char*) pelem->GetText());
            else
                strcpy(cfg.baselock, (char*) "disabled");



            pelem = elem->FirstChildElement("api");
            if (pelem)
                strcpy(cfg.api, (char*) pelem->GetText());
            else
                strcpy(cfg.api, (char*) "disabled");

            pelem = elem->FirstChildElement("recycler");
            if (pelem)
                strcpy(cfg.recycler, (char*) pelem->GetText());
            else
                strcpy(cfg.recycler, (char*) "disabled");

            pelem = elem->FirstChildElement("printer");
            if (pelem)
                strcpy(cfg.printer, (char*) pelem->GetText());
            else
                strcpy(cfg.printer, (char*) "disabled");

            pelem = elem->FirstChildElement("tampersw");
            if (pelem)
                strcpy(cfg.tampersw, (char*) pelem->GetText());
            else
                strcpy(cfg.tampersw, (char*) "disabled");

		}


		if(elemName == "logs")
		{
			pelem = elem->FirstChildElement("mysql");
			if (pelem)
				strcpy(cfg.logdbf, (char*) pelem->GetText());
			else
				strcpy(cfg.logdbf, (char*) "disabled");

			pelem = elem->FirstChildElement("remotedbf");
			if (pelem)
				strcpy(cfg.logremotedbf, (char*) pelem->GetText());
			else
				strcpy(cfg.logremotedbf,  (char*)"disabled");

			pelem = elem->FirstChildElement("file");
			if (pelem)
				strcpy(cfg.logfile, (char*) pelem->GetText());
			else
				strcpy(cfg.logfile,  (char*)"disabled");

		}

		if(elemName == "remote")
		{
			pelem = elem->FirstChildElement("clientportal");
			if (pelem)
				strcpy(cfg.clientportal, (char*) pelem->GetText());
			else
				strcpy(cfg.clientportal,  (char*)"disabled");

			pelem = elem->FirstChildElement("maint");
			if (pelem)
				strcpy(cfg.maintportal, (char*) pelem->GetText());
			else
				strcpy(cfg.maintportal, (char*) "disabled");

			pelem = elem->FirstChildElement("dbf");
			if (pelem)
				strcpy(cfg.dbfportal, (char*) pelem->GetText());
			else
				strcpy(cfg.dbfportal, (char*) "disabled");
		}


		if (elemName == "remoteDBF")
		{

			pelem = elem->FirstChildElement("ip");
			if (pelem)
				strcpy(cfg.iptxt, (char*) pelem->GetText());
			else
				strcpy(cfg.iptxt, (char*) "localhost");

			pelem = elem->FirstChildElement("user");
			if (pelem)
				strcpy(cfg.usertxt, (char*) pelem->GetText());
			else
				strcpy(cfg.usertxt,  (char*) "fkinguser101");

			pelem = elem->FirstChildElement("password");
			if (pelem)
				strcpy(cfg.pwtxt, (char*) pelem->GetText());
			else
				strcpy(cfg.pwtxt, (char*) "glimpbust345");

			pelem = elem->FirstChildElement("database");
			if (pelem)
				strcpy(cfg.dbftxt, (char*) pelem->GetText());
			else
				strcpy(cfg.dbftxt, (char*) "fking");


			pelem = elem->FirstChildElement("port");
			if (pelem)
				strcpy(cfg.porttxt, (char*) pelem->GetText());
			else
				strcpy(cfg.porttxt, (char*) "3306");

			if (!silent)
				printf("REMOTE:: ip:%s  user:%s  pw:%s dbf:%s  port:%s  \n",cfg.iptxt,cfg.usertxt,cfg.pwtxt,cfg.dbftxt,cfg.porttxt);

			// now populate our local object
			strcpy(remoteDBF.user,cfg.usertxt);
			strcpy(remoteDBF.password,cfg.pwtxt);
			strcpy(remoteDBF.database,cfg.dbftxt);
			remoteDBF.port= atoi(cfg.porttxt);
			strcpy(screenOrientation,cfg.screentxt);

		}



	}




	if (!silent)
		PrintConfig();

	// do the housekeeping
	root->Clear();
	pelem->Clear();
	doc.Clear();
	return true;
}




void PrintConfig(void)
{

	printf("\n\nCONFIGURATION-----------------------------------\n");
	printf("screentxt: %s\n",cfg.screentxt);

	//dbf
	printf("DBF\n");
    printf("iptxt: %s\n", cfg.iptxt);
    printf("usertxt: %s\n", cfg.usertxt);
    printf("pwtxt: %s\n", cfg.pwtxt);
    printf("dbftxt: %s\n", cfg.dbftxt);
    printf("porttxt: %s\n", cfg.porttxt);


	// devices
	printf("DEVICES\n");
    printf("validator1: %s\n",cfg.validator1);
    printf("validator2: %s\n",cfg.validator2);
    printf("ucd1: %s\n", cfg.ucd1);
    printf("ucd2: %s\n", cfg.ucd2);

    printf("utd: %s\n", cfg.utd);


	printf("outter: %s\n",cfg.outterlock);
	printf("inner: %s\n",cfg.innerlock);
	printf("shutter: %s\n",cfg.shutterlock);
	printf("sidecar: %s\n",cfg.sidecarlock);
	printf("base: %s\n",cfg.baselock);

    printf("api: %s\n",cfg.api);
    printf("recycler: %s\n", cfg.recycler);
    printf("printer: %s\n", cfg.printer);
    printf("tampersw: %s\n", cfg.tampersw);

	// logs
	printf("LOGS\n");
    printf("logdbf: %s\n", cfg.logdbf);
    printf("logfile: %s\n", cfg.logfile);
    printf("logremotedbf: %s\n", cfg.logremotedbf);

    // remote
	printf("REMOTE\n");
    printf("clientportal: %s\n", cfg.clientportal);
    printf("mainportal: %s\n", cfg.maintportal);
    printf("dbfportal: %s\n", cfg.dbfportal);

	printf("\n-----------------------------\n\n");

}





/*
	kill all connections
	1. USB
	2. sockets

*/
void KillConnections(void)
{
}


/*
	read the screen file for resolution
	format is...

	#comments are allowed (max line len=100)
	H=1280
	V=800

*/


void GetScreenRes(void)
{
	FILE *file;
	char screenfile[]="screen";
	char mystring[100];
	char * pch;

	file=fopen(screenfile,"r");
	if (file == NULL)
	{
		screenres.horiz=1280;
		screenres.vert=800;
		return;
	}


	while (fgets(mystring,50, file) )
	{
		if (mystring[0]=='#') continue;		// skip comments

		pch = strstr(mystring,"=");
		if (pch != NULL)
		{
			if (mystring[0]=='H')
			{
				screenres.horiz = atoi(pch+1);
			}

			if (mystring[0] == 'V')
			{
				screenres.vert = atoi(pch+1);
			}
		}
	}


	fclose(file);
}


#define MAX_UTD_COLS 8
struct
{
	int col;
	string tube_name;
	int tube_value;
	int tube_count;
	float dollars;
} utd_inv[MAX_UTD_COLS];


void Get_UTD_Data(void)
{
    char query[]="SELECT col,tube_name,tube_value,tube_count  FROM utd_denom";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer
//    printf("Data Returned: %s\n",localDBF.row[1]);

	for (int n=0; n < numrows; n++)
	{
		utd_inv[n].col= atoi(localDBF.row[0]);				// column
        utd_inv[n].tube_name= string(localDBF.row[1]);    	// tube_name
		utd_inv[n].tube_value= atoi(localDBF.row[2]);		// tube_value
		utd_inv[n].tube_count= atoi(localDBF.row[3]);		// tube_count
		utd_inv[n].dollars= (utd_inv[n].tube_count * utd_inv[n].tube_value) /100.0;
//		printf("col %d:: %s  Dollars %4.2f\n",utd_inv[n].col,utd_inv[n].tube_name.c_str(),  utd_inv[n].dollars);
		GetRow(&localDBF);	// get next row
	}

}


struct
{
	string code;
	string name;
	int active;
} langs[10];


/*
	read the langs table of all supported languages and populate
	langs[] struct

*/
void GetLangs(void)
{
    char query[]="SELECT code,name,active  FROM langs";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer

	for (int n=0; n < numrows; n++)
	{
		langs[n].code = string(localDBF.row[0]);
		langs[n].name = string(localDBF.row[1]);
		langs[n].active = atoi(localDBF.row[2]);

		printf("Lang: %s  Code:%s  Active: %d\n",langs[n].name.c_str(),langs[n].code.c_str(),langs[n].active);
		GetRow(&localDBF);	// get next row
	}

}




/*
	this markup works for GTK labels
	markup the XML help string based on embedded attributes in the XML text

	ATTRIBUTES PARSED
	[b]  bold text  [/b]
*/

string Markup(string str)
{
	string markup_string;

	markup_string="<span weight='bold' fgcolor='#DC143C'>";
	ReplaceAll( str, "[b]", markup_string);
	ReplaceAll( str, "[/b]", "</span>");				// remove the trailing tag

	return str;
}






/*
	this markup works for GTK text_buffers (like for a textview widget)

	We locate the tags and replace them with empty space, however, we mark
	the offset in the buffer and set an Iter for both the start and end tags.
	We then apply the proper tag to the start Iter and end Iter for that text region

	We create the BOLD tag once, via a static boolean. Creating more than one spawns a GTK warning

	ATTRIBUTES LIST
	[b]  bold text  [/b]
	[i] italic text [/i]
	[red] red text [/red]
	[green] grn txt [/green]

*/


/*
void MarkupBody(void)
{
	string markup;
	char *ptr;
	gint offset=-0;
	char buffer[5000];
	GtkTextIter sIter, eIter;
	static bool tagged=FALSE;

	// remove all tags from the string so we can place the string in the text_buffer
	// then we will work out where the tags below via iters'
	markup = HELP[help_window_page].body ;

	// do one for each tag supported
	ReplaceAll( markup , "[b]", "" );
	ReplaceAll( markup, "[/b]", "" );
	ReplaceAll( markup, "[red]", "" );
	ReplaceAll( markup, "[/red]", "" );
	ReplaceAll( markup, "[green]", "" );
	ReplaceAll( markup, "[/green]", "" );
	ReplaceAll( markup, "[i]","");
	ReplaceAll( markup, "[/i]","");

	// now insert the cleaned up (plain text) version into the text_buffer
	gtk_text_buffer_set_text( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), markup.c_str(), markup.length() );

	// if we haven't already, then create the BOLD tag
	if (! tagged)
	{
		gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),
				"bold",
				"weight",
				PANGO_WEIGHT_BOLD,
				NULL);

		gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),
				"red",
				"foreground",
				"#FF0000",
				NULL);

		gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),
				"green",
				"foreground",
				"#22DD22",
				NULL);


		gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),
				"italic",
				"style",
				PANGO_STYLE_ITALIC,
				NULL);

		tagged=TRUE;
	}

	// get the original marked up version of the text, so we can determine where to apply any tags
	strcpy(buffer,HELP[help_window_page].body.c_str() );

	#define BOLD_TAG_LEN 3		// [b]
	#define RED_TAG_LEN 5		// [red]
	#define GREEN_TAG_LEN 7		// [green]
	#define ITALIC_TAG_LEN 3	// [i]

	// find all occurrences of a tag
	// NOTE: if NO tags are found, we simply fail the first condition below and fall out
	//		this is OK, since we have already displayed the plain text version of the target string
	while  ( ptr = strstr( buffer, "[") ) 
	{
		// dispatcher based on next character
		switch ( *(ptr+1))
		{
			// HANDLE THE "ON" TAGS
		case 'b':
		case 'B':
			// BOLD TAG ======================================================================================
			// ptr points to the first character of "[b]", snag the iterator
			// we found a "[b]" tag
			offset = ptr - &buffer[0];
			gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &sIter, offset);
			// now copy the rest of the string past the tag, essentially gobbling up the tag
			memmove( ptr, ptr+BOLD_TAG_LEN, strlen( ptr+BOLD_TAG_LEN ) );
			break;
		case 'r':
		case 'R':
			// RED TAG ======================================================================================
			// ptr points to the first character of "[r]", snag the iterator
			// we found a "[r]" tag
			offset = ptr - &buffer[0];
			gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &sIter, offset);
			// now copy the rest of the string past the tag, essentially gobbling up the tag
			memmove( ptr, ptr+RED_TAG_LEN, strlen(ptr+RED_TAG_LEN) );
			break;

		case 'g':
		case 'G':
			// GREEN TAG ======================================================================================
			// ptr points to the first character of "[g]", snag the iterator
			// we found a "[g]" tag

			offset = ptr - &buffer[0];
			gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &sIter, offset);
			// now copy the rest of the string past the tag, essentially gobbling up the tag
			memmove( ptr, ptr+GREEN_TAG_LEN, strlen(ptr+GREEN_TAG_LEN) );
			break;

		case 'i':
		case 'I':
			// ITALIC TAG ======================================================================================
			// ptr points to the first character of "[i]", snag the iterator
			// we found a "[i]" tag
			offset = ptr - &buffer[0];
			gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &sIter, offset);
			// now copy the rest of the string past the tag, essentially gobbling up the tag
			memmove( ptr, ptr+ITALIC_TAG_LEN, strlen( ptr+ITALIC_TAG_LEN ) );
			break;


		case '/':
			// HANDLE THE "OFF" TAGS
			switch( *(ptr+2) )
			{
			case'b':
			case 'B':
				// we found a close tag
				offset = ptr - &buffer[0];
				gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &eIter, offset);
				// now copy the rest of the string past the tag, essentially gobbling up the tag
				memmove( ptr, ptr+BOLD_TAG_LEN+1, strlen( (ptr+BOLD_TAG_LEN)) );
				// we have both the start and end Iters, so lets apply the bold tag to that region of text
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),"bold", &sIter, &eIter);
				break;

			case 'r':
			case 'R':
				// we found a close tag
				offset = ptr - &buffer[0];
				gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &eIter, offset);
				// now copy the rest of the string past the tag, essentially gobbling up the tag
				memmove( ptr, ptr+RED_TAG_LEN+1, strlen(ptr+RED_TAG_LEN+1) );
				// we have both the start and end Iters, so lets apply the bold tag to that region of text
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),"red", &sIter, &eIter);
				break;
			case 'g':
			case 'G':
				// we found a close tag
				offset = ptr - &buffer[0];
				gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &eIter, offset);
				// now copy the rest of the string past the tag, essentially gobbling up the tag
				memmove( ptr, ptr+GREEN_TAG_LEN+1, strlen(ptr+GREEN_TAG_LEN+1) );
				// we have both the start and end Iters, so lets apply the bold tag to that region of text
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),"green", &sIter, &eIter);
				break;

			case 'i':
			case 'I':
				// we found a close tag
				offset = ptr - &buffer[0];
				gtk_text_buffer_get_iter_at_offset( GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer), &eIter, offset);
				// now copy the rest of the string past the tag, essentially gobbling up the tag
				memmove( ptr, ptr+ITALIC_TAG_LEN+1, strlen(ptr+ITALIC_TAG_LEN+1) );
				// we have both the start and end Iters, so lets apply the bold tag to that region of text
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(app_ptr->help_body_textbuffer),"italic", &sIter, &eIter);
				break;
			default:
				// this section is for an invalid tag or lack of closing match
				ptr++;
			} // end switch
			break;

			// in the event we found a "[", but don't recognize the next character, then we fall through
			// to here, so increment the pointer and continue the search
			// in the above cases, where we recognized the next character, that character has now
			// been replaced and we can simply continue the search from there
		default:
			ptr++;
		} // endswitch


	} //endwhile

}

*/


/*
#include <iostream>
#include <functional>
#include <iterator>

//typedef std::function<void(uint32_t)> cb_t;
typedef std::function<void(int)> cb_t;


struct cb_event_t
	{
	std::function<void(int)> cb;
	uint32_t handle;
	};

std::vector<cb_event_t> callbacks_;


class EventDriver {
public:

    EventDriver(uint32_t val) : val_(val), callbacks_() { }


    // Register a callback.
    void register_callback(const cb_t &cb,uint32_t handle)
    {
        // add callback to end of callback list
        callbacks_.push_back({cb,handle});
    }

//	void remove_callback(const cb_t &cb)
void remove_callback(uint32_t handle)
	{


		vector<cb_event_t>::iterator ptr;
		for (ptr = callbacks_.begin(); ptr < callbacks_.end(); ptr++)
		{
			if (ptr->handle == handle)
			{
				callbacks_.erase(ptr);
cout << ptr->handle << " ";
			}

//			cout << ptr.cb << " ";
		}

	}

	// get the number of elements in the vector
	int size(void)
	{
		return callbacks_.size();
	}

	// return number of elements that can be held in currently allocated storage
	int capacity(void)
	{
		return callbacks_.capacity();
	}

	int isempty(void)
	{
		return callbacks_.empty();	// returns true or false
	}

	// resize the vector to hold count elements
	void resize(int count)
	{
		callbacks_.resize(count);
	}

	void clear(void)
	{
		callbacks_.clear();
	}

	// remove the last item in the list
	void removelast(void)
	{
		callbacks_.pop_back();
	}

    /// Call all the registered callbacks.
    void callback() const
    {
		uint32_t handle=0;
		// iteraet thru callback list and call each one
		for (const auto &cb: callbacks_)
		{
			if (cb.handle == handle)
			{
				cb.cb(val_);
			}
		}

    }

private:
    /// Integer to pass to callbacks.
    uint32_t val_;
    /// List of callback functions.
    std::vector<cb_event_t> callbacks_;
};





//std::vector<cb_t> callbacks_;
EventDriver ed(0xDEADBEEF); //create basic driver instance


void testcb(void)
{
//	BasicDriver bd(0xDEADBEEF); //create basic driver instance
	ed.register_callback(&UpdateUTDInventory,0);
}


void ExecCallBacks(void)
{

	ed.callback();
}


void removeCallBack(void)
{
//	ed.remove_callback(&UpdateUTDInventory,0);
}


*/

