/*
	Program: VSafe
	Module: vsafe.cpp (primary module)
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



======================================
building a scrolling listbox in GLADE
======================================

1. create a GtkBox in your window
2. to that box add GtkScrolledWindow (containers tab in GLADE)
	a. set scrollbar policy=always
	b. set max content height= some number
	c. add an adjustment widget
3. add a GtkViewPort to the above
4. add your_listbox to the above
5. use AddListItem(GtkWidget * list, char * txt) to add items into your list
6. be sure to use _show_all when displaying the window
7. see ShowUserList() for example usage

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
#include "config.inc"

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
#include "api.inc"


//mysql  Ver 14.14 Distrib 5.7.20, for Linux (x86_64) using  EditLine wrapper

// xml file produced by glade
#define XML_PORTRAIT "xml/port.glade"
//#define XML_LANDSCAPE "xml/land.glade"
#define XML_LANDSCAPE "xml/safe.glade"

char XML_FILE[50];

string SerializeConfig(void);
bool ConfigSetup(bool silent);
void PrintConfig(void);


// color scheme resource file
#define RES_FILE "../res/flowformrc"


// CLOUD CONNECTION
SOCKET * cloud_server = new SOCKET(TCP_CONNECTION); // instantiate our cloud socket
void ConnectCloud(void);
bool SendCloud(string pkttype, string filename, char * msg);

 // tbl can be a single table name or several tbls separated by space
// for either of the following functions
bool SendTableToServer(string tbl);
string DumpSQLTable(string tbl);


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




string current_user;
string current_pw;

struct user_rec
{
	string username;
	string password;
	string lang;
	string fname;
	string lname;
	string user_level;
	string dept;
	string created;
	string lastmodified;
	string active;
	string id;
};


user_rec current_user_rec;

bool WriteUserRecord(void);
bool ReadUserRecord(string username,string password);
bool CheckUser(string username);
void PrintUserRecord(void);

#define MAX_USER_LEVELS 20


// these are used on the permissions screen, grid.. and holds a pointer to the various checkboxes onscreen
GtkWidget * ulevel_lbl[MAX_USER_LEVELS];
GtkWidget * ulevel_cr[MAX_USER_LEVELS];
GtkWidget * ulevel_au[MAX_USER_LEVELS];
GtkWidget * ulevel_eu[MAX_USER_LEVELS];
GtkWidget * ulevel_dep[MAX_USER_LEVELS];
GtkWidget * ulevel_rpt[MAX_USER_LEVELS];


struct
{
    string name;
    int level;
} user_levels[MAX_USER_LEVELS];
int user_level_count;

struct
{
	string department;
	string lastmodified;
	string id;
} depts[25];
int dept_count;

struct
{
	int user_level;
	int cr;		// content removal
	int au;		// add users
	int eu;		// edit users
	int dep;	// deposits
	int rpt;	// reports
} perms[MAX_USER_LEVELS];
int perm_count;
bool perms_saved=FALSE;

int perm_grid_row_count=0;	// counts rows added to the perms_grid, used to prevent adding checkboxes a second time

bool user_info_saved=FALSE;
void ShowPermSave(void);

void ShowAddUserLevel(void);
#define BASE_USER_LEVEL 200	// custom user levels start here
int GetNextUserLevel(void);


bool settingperms;
void SetPermValues(void);

int GetUserLevelIndex(int user_level);
void GetPerms(void);
bool WritePerms(void);
void GetUserLevels(void);
void GetDepartments(void);

void PopulateDeptCombo(void);
void PopulateUserLangsCombo(void);
void PopulateUserLevelCombo(void);
void SetCurrentUserSelections(void);

unsigned int user_count;
int GetUserCount(void);	// sets above var
user_rec *users=NULL;
void GetAllUsers(void);





// pid.cpp
bool CheckRunning(int pid_ID);		// check to see if app is running, if so, return TRUE
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
#define ADMIN 		99
#define MGR 		50
#define CASHIER		10
#define GOD_MODE 	999


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

void ShowMainMenu(void);
void ShowLoad(void);
void ShowConfig(void);
void ShowMaint(void);
void ShowUTDMaint(void);
void ShowMEIMaint(void);
void ShowLogin(void);
void ShowSplashWindow();
void ShowStatus(string status);
void ShowAdmin(void);
void ShowDeposit(void);
void ShowReports(void);

void ShowDepositCash(void);
void AddDeposit(char *txt);


void ShowUserList(void);
void AddListItem(GtkWidget * list, char *txt);
static GtkWidget * create_row(const gchar *txt);
const char* GetRowText(void);


void ShowPerms(void);
void AddPermGrid(void);

bool global_adding_user=FALSE;
void ShowUser(bool adding_user);
void ShowUserSavedStatus(void);

void ShowSettings(void);

void GetPermissionFields(void);

void BackSpace(void);
void TabButton(void);

bool logged_in=FALSE;
bool ValidateUser(char *user,char *pw);

void StoreInput(char x);	// store the input from the keyboard to input_text[]
void SetEntryText(void);	// set input_text[] into the db.target widget

// used by the above 2 functions
char input_text[100];
int input_text_index=0;


void Get_UTD_Data(void);


//GtkWidget * active_window;	// holds a pointer to the active window


/*
struct window
{
	GtkWidget * window=NULL;
	GtkWidget * key_target=NULL;// target widget for keyboard or keypad (set to NULL if no key/num pad onscreen)
	GtkWidget * output_target;	// target widget for output from keyboard or keypad (changed via TAB key)
	bool keyboard_avail=FALSE;	// says if keyboard is available or not (determines visibility of ABC/123 button)
	bool ucase=FALSE;			// determines if CASE button is visible or not
};
*/

/*
    widget names for key/num pad usage
    ----------------------------------------------------------

    WIDGET                  HOME WINDOW
    ========================================
    keyboard_grid           keyboard_window
    numpad_grid             numpad_window
*/


// support functions using the following kb structure

void InstallKB(void);			// install the kb according to kb.struct
void ReHomeKB(void);			// put the kb back to its home window
void SetKeyboardCase(void);
void ShowCaseBtn(void);
void ShowAbcBtn(void);



//window current_window;

// used for ABC/123 button with keypad/keyboard (kb.abc123)
#define KEYBOARD 1
#define KEYPAD   0


struct
{
	GtkWidget * attached;		// widget kb is attached to currently
	GtkWidget * active;			// which kb is active, keyboard or numpad
	GtkWidget * target;			// output target
	bool allow_alpha;			// allow keyboard or is it numpad only
	bool allow_case;			// allow U/L case or not
	bool is_upper=FALSE;		// current status of keyboard TRUE = uppercase, FALSE = lower case
	int abc123=KEYBOARD;			// KEYBOARD | KEYPAD
} kb;

// the following sets the output target for chars from the keyboard or keypad
// used to get vars in the window struct
enum display
{
    userlogin,
    username,
    userfname,
    userlname,
};




struct
{
    string code;
    string name;
    int active;
    int id;
} langs[10];
int lang_count;	// count of active langs in dbf

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

/*
	MOVED TO VSAFE.INC, typedef struct {} config;
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
    bool validator_left;
    bool validator_right;
	bool validator_ucd;
    char ucd1[10];
    char ucd2[10];
	char utd[10];
	bool pelicano=FALSE;
	bool gsr50=FALSE;
    char outterlock[10];
    char innerlock[10];
    char shutterlock[10];
    char sidecarlock[10];
    char baselock[10];
    char printer[10];
    char tampersw[10];


    // logs
    char logdbf[10];
    char logenabled[10];		// enabled/disabled
	char logfile[40];
    char logremotedbf[10];

    // remote
    char clientportal[10];	// enabled/disabled
    char maintportal[10];
    char dbfportal[10];

	// remoteserver
	char cloud_ip[20];
	char cloud_port[10];

	// timeouts
	char validator_timeout[10];

	// api
	char api_port[10];
	bool api_enabled=FALSE;
} cfg;
*/

config cfg;



int GetAPIport(void);

void API_Handler(void);





/*
	cleanup here.. free mem etc.
*/

void local_cleanup(void)
{

	delete[] users;
	KillConnections();


}



//extern "C" void on_login_window_destroy(GtkObject *object, gpointer data)
extern "C" void on_login_window_destroy()
{

	local_cleanup();
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

if ( cfg.utd)
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

	init_validators();    //init validators (in usb_gateway)

/*
=============================================================================================
end of Initiating the validators in the USB Gateway if Enabled
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


/*
#ifdef DISABLE_LOGIN
	user_code=GOD_MODE;
#else
	user_code = NO_USER;
	gtk_widget_show(app->login_dialog);
#endif
*/


//	ShowMainMenu();
//	gtk_widget_show_all(app->main_menu);
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

	// if enabled, connect to the remote server
	ConnectCloud();

	string tbl="users devices";
//	SendTableToServer(tbl);

    if (cfg.api_enabled)		SetupAPI();

	#ifdef FULLSCREEN
//		gtk_window_fullscreen( GTK_WINDOW(app_ptr->main_menu) );
	#endif

	ShowSplashWindow();
//	ShowStatus("this is a test message");
	REPARENT(app_ptr->numpad_window,app_ptr->login_pad_target,app_ptr->numpad_grid)

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


/*
	see if the username exists in the DBF, if so RETURN TRUE, else return FALSE

*/
bool CheckUser(string username)
{
    char query[200];
	sprintf(query,"SELECT username FROM users WHERE username='%s';",username.c_str());
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);
	if (numrows ==0) return FALSE;

	return TRUE;
}



/*
	get current users record
	if password=="", then search for username only
	RETURNS: TRUE if found and current_user_rec struct filled in
	else returns FALSE
*/
bool ReadUserRecord(string username,string password)
{
	char query[200];
	if (password != "")
	    sprintf(query,"SELECT username,lang,fname,lname,user_level,dept,lastmodified,active,id FROM users WHERE username='%s' AND pw=PASSWORD('%s');",username.c_str(),password.c_str());
	else
        sprintf(query,"SELECT username,lang,fname,lname,user_level,dept,lastmodified,active,id FROM users WHERE username='%s';",username.c_str() );

    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);
	if (numrows !=0)
	{
	    current_user_rec.username = string(localDBF.row[0]);
        current_user_rec.lang = string(localDBF.row[1]);
	    current_user_rec.fname =  string(localDBF.row[2]);
	    current_user_rec.lname = string(localDBF.row[3]);
	    current_user_rec.user_level = localDBF.row[4];
	    current_user_rec.dept = string(localDBF.row[5]);
	    current_user_rec.lastmodified = string(localDBF.row[6]);
	    current_user_rec.active = localDBF.row[7];
	    current_user_rec.id = localDBF.row[8];
//PrintUserRecord();
		return TRUE;
	}

	return FALSE;
}


void PrintUserRecord(void)
{
	printf("print user record\n");
	printf("Username: %s\n",current_user_rec.username.c_str() );
    printf("First name: %s\n",current_user_rec.fname.c_str() );
    printf("Last name: %s\n",current_user_rec.lname.c_str() );
    printf("Lang: %s\n",current_user_rec.lang.c_str() );
    printf("User Level: %s\n",current_user_rec.user_level.c_str() );
    printf("Dept: %s\n",current_user_rec.dept.c_str() );
    printf("Last Modified: %s\n",current_user_rec.lastmodified.c_str() );
    printf("Active: %s\n",current_user_rec.active.c_str() );
    printf("ID: %s\n",current_user_rec.id.c_str() );



}


/*
	get the departments from DBF:depts table
	store to depts[] struct

struct
{
    string department;
    string lastmodified;
    string id;
} depts[25];

*/

void GetDepartments(void)
{
	char query[200]="SELECT department,lastmodified,id FROM depts";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);
	dept_count=numrows;

	if (numrows !=0)
	{
		for (int n=0; n < numrows; n++)
		{
			depts[n].department= string(localDBF.row[0]);
			depts[n].lastmodified = string(localDBF.row[1]);
			depts[n].id = string(localDBF.row[2]);
            GetRow(&localDBF);
		}
	}
}


/*
	read all permission types from the DBf:perms
	and store into perms struct (indexed by user_level)

*/

void GetPerms(void)
{
    char query[200]="SELECT user_level,cr,add_users,edit_users,deposits,reports FROM perms;";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);
	perm_count=numrows;

	for (int n=0; n < numrows; n++)
	{
		perms[n].user_level= atoi(localDBF.row[0]);
		perms[n].cr = atoi(localDBF.row[1]);
		perms[n].au= atoi(localDBF.row[2]);
		perms[n].eu = atoi(localDBF.row[3]);
		perms[n].dep = atoi(localDBF.row[4]);
		perms[n].rpt = atoi(localDBF.row[5]);
		GetRow(&localDBF);
	}

}


/*
	write data from perms[] back to DBF
	perm_count has number of elements

	RETURNS: TRUE if OK, else FALSE
*/
bool WritePerms(void)
{
	char query[200];
	bool res=TRUE;

	for (int n=0; n < perm_count; n++)
	{
		sprintf(query,"UPDATE perms SET cr='%d', add_users='%d', edit_users='%d',deposits='%d',reports='%d' WHERE user_level='%d';",perms[n].cr,perms[n].au,perms[n].eu,perms[n].dep,perms[n].rpt,user_levels[n].level);
		// returns 0 on success, else error code
	    int result =  QueryDBF(&localDBF,query);
		if (result !=0)
		{
			res=FALSE;	// flag the error
			// if there was an error, gen_buffer has the error msg
			WriteSystemLog(gen_buffer);
		}

	}

	return res;


}




/*
	get the user levels from the DBF:user_levels table
	store to user_levels[] struct

*/

void GetUserLevels(void)
{
	char query[200]="SELECT name,level FROM user_levels";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);
	user_level_count=numrows;
    if (numrows !=0)
    {
		for (int n=0; n < numrows; n++)
		{
			user_levels[n].name=string(localDBF.row[0]);
			user_levels[n].level=atoi(localDBF.row[1]);

//printf("LEVEL: %s\n",user_levels[n].name.c_str());
			GetRow(&localDBF);
		}
	}
}

/*
	write the edited user record  back to DBF

	RETURNS; TRUE on success, else FALSE

struct
{
    string username;
    string password;
    string lang;
    string fname;
    string lname;
    string user_level;
    string dept;
    string lastmodified;
    string active;
    string id;
} current_user_rec;

  `username` varchar(20)  NOT NULL,
  `password` varchar(80) NOT NULL,
  `lang` varchar(2) NOT NULL DEFAULT 'en',
  `fname` varchar(30) NOT NULL,
  `lname` varchar(30) NOT NULL,
  `user_level` int(5) NOT NULL DEFAULT 0,
  `dept` varchar(30),
  `created` datetime DEFAULT NOW(),
  `lastmodified` datetime DEFAULT NOW() ON UPDATE NOW(),
  `active` tinyint NOT NULL DEFAULT 0,
  `id` int(5) COLLATE utf8_bin auto_increment,

*/
bool WriteUserRecord(void)
{
printf("WRITE USER RECORD\n");
	char query[400];
	sprintf(query,"UPDATE users SET username='%s', lang='%s',fname='%s',lname='%s',user_level='%s',dept='%s',active='%s' WHERE id='%s'; ", current_user_rec.username.c_str(), current_user_rec.lang.c_str(), current_user_rec.fname.c_str(), current_user_rec.lname.c_str(), current_user_rec.user_level.c_str(), current_user_rec.dept.c_str(), current_user_rec.active.c_str(), current_user_rec.id.c_str() );
//TODO - re-activate actual SAVE of user info
printf("QUERY: %s\n",query);
return TRUE;

	// returns 0 on success, else non-zero
	int result =  QueryDBF(&localDBF,query);



	if (result ==0)
		return TRUE;
	else
	{
		// error
		sprintf(gen_buffer,"WriteUserRecord:: DBF ERROR: %s << failed",query );
		WriteSystemLog(gen_buffer);
		return FALSE;
	}

}




//=======================================================================
//                  START SETTINGS WINDOW
//=======================================================================

void ShowSettings(void)
{
    gtk_widget_show(app_ptr->settings_window);
}

extern "C" bool on_settings_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->settings_window);
}

extern "C" bool on_lock_config_btn_clicked( GtkButton *button, AppWidgets *app)
{
    ShowConfig();
    printf("LOCK CONFIG\n");
}


//TODO chanfge_pw btn
extern "C" bool on_change_pw_btn_clicked( GtkButton *button, AppWidgets *app)
{
}


//TODO sound_level_btn
extern "C" bool on_sound_level_btn_clicked( GtkButton *button, AppWidgets *app)
{
}



//=======================================================================
//                  END SETTINGS WINDOW
//=======================================================================





//=======================================================================
//                  START NEW USER LEVEL WINDOW
//=======================================================================

/*
	display the screen for adding a new user level into the DBF

*/
void ShowAddUserLevel(void)
{

	// install/show the keyboard
    kb.attached= app_ptr->perm_kb_target;   // new hoome for keyboard/numpad widget
    kb.active=app_ptr->keyboard_grid;       // keyboard_grid or numpad_grid
    kb.target=app_ptr->new_userlevel_entry; // desination for output text
    kb.allow_alpha=TRUE;
    kb.allow_case=FALSE;	// allow upper AND lower case
    kb.is_upper=FALSE;		// current status of keyboard TRUE = uppercase, FALSE = lower case
    kb.abc123=KEYBOARD;    // KEYPAD | KEYBOARD
    ShowCaseBtn();      // show/hide CASE btn
    ShowAbcBtn();		// show/hide abc/123 btn
    InstallKB();        // reparent the kb widget into the current window


	gtk_widget_show(app_ptr->add_user_level_window);
}


	// OK button, save the new user level
extern "C" bool on_add_userlevel_ok_btn_clicked( GtkButton *button, AppWidgets *app)
{
	string msg;
	int result;
	int numrows;
	char query[200];
/*
	1. get the text from new_userlevel_entry
	2. check perms tbl to make sure it doesnt already exist
	3. add a new rec to perms tbl
*/
	const gchar * ulevel_name;
	// 1. get the text name of new user level
    ulevel_name = gtk_entry_get_text(GTK_ENTRY(app_ptr->new_userlevel_entry));

//printf("NEW NAME:%s\n",ulevel_name);
	//2. check perms tble to make sure new level name doesnt exist
    sprintf(query,"SELECT name FROM user_levels WHERE name='%s';",ulevel_name);
	// returns 0 on success, else nonzero err code
    result =  QueryDBF(&localDBF,query);
    numrows = GetRow(&localDBF);

	// in the case of not finding the new user level name, both numrows and result = 0
	//	printf("NUMROWS:: %d RES:: %d\n",numrows,result);

//printf("NUMROWS:%d\n",numrows);
	if (numrows >0 )	// it already exists
	{
		sprintf(gen_buffer,"User level %s already exists, try another",ulevel_name);
		ShowStatus(string(gen_buffer));
		return FALSE;
	}

//#define BASE_USER_LEVEL 200

	int next_level = GetNextUserLevel();
printf("NEXT LEVEL: %d  NEW NAME:%s\n",next_level,ulevel_name);

	// 3. add a new record to user_level tbl
	sprintf(query,"INSERT INTO user_levels SET name='%s', level='%d';",ulevel_name, next_level);
printf("ADD QUERY: %s\n",query);
    result =  QueryDBF(&localDBF,query);

        gtk_widget_hide(app_ptr->add_user_level_window);
		ReHomeKB();
}


// CANCEL button
extern "C" bool on_add_userlevel_cancel_btn_clicked( GtkButton *button, AppWidgets *app)
{
        gtk_widget_hide(app_ptr->add_user_level_window);
        ReHomeKB();
}


/*
	return the next available custom user_level
*/
int GetNextUserLevel(void)
{
//printf("GETNEXTUSERLEVEL\n");
	int level;
	char query[200];
	sprintf(query,"SELECT MAX(level) FROM user_levels WHERE level >'%d' AND level < '999';",BASE_USER_LEVEL);
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);

	if (numrows==0)
	{
		return BASE_USER_LEVEL;
	}

//printf("QUERY: %s\n",query);
//printf("2NUMROWS:%d\n",numrows);

if (localDBF.row[0] == NULL)
	level=BASE_USER_LEVEL-1;
else
	level = atoi(localDBF.row[0]);	// get last entered custom value

	return ++level;
}


//=======================================================================
//                  END NEW USER LEVEL WINDOW
//=======================================================================






//=======================================================================
//                  START PERMS WINDOW
//=======================================================================

void ShowPermSave(void)
{
	string msg;

	if (perms_saved)
	    msg = getMessage(372,FALSE); // "saved"
	else
       msg = getMessage(373,FALSE); // "not saved"

    gtk_label_set_label(GTK_LABEL(app_ptr->perms_saved_lbl),msg.c_str() );
}


void ShowPerms(void)
{
	string msg;
	GtkLabel *wid;

	perms_saved=TRUE;
	ShowPermSave();
	msg = getMessage(255,FALSE); // "PERMISSIONS"
    gtk_label_set_label(GTK_LABEL(app_ptr->perms_title),msg.c_str() );

	msg = getMessage(50,FALSE); // "CLOSE"
    gtk_button_set_label(GTK_BUTTON(app_ptr->perm_close_btn),msg.c_str() );

    msg = getMessage(357,FALSE); // "SAVE"
    gtk_button_set_label(GTK_BUTTON(app_ptr->perm_save_btn),msg.c_str() );



	AddPermGrid();			// add lbls and checkboxes to perms_grid based on whats in DBF
	SetPermValues();		// set initial values of checkboxes basedup on the DBF
    gtk_widget_show_all(app_ptr->perms_window);

    perms_saved=TRUE;
    ShowPermSave();

}


/*
	add a new permssion to the list

*/
extern "C" bool on_perm_add_btn_clicked( GtkButton *button, AppWidgets *app)
{
	ShowAddUserLevel();

}


/*
	save the permissions as selected on the screen
*/
extern "C" bool on_perm_save_btn_clicked( GtkButton *button, AppWidgets *app)
{
	if (WritePerms())
		getMessage(310,FALSE);
	else
		getMessage(311,FALSE);

	ShowStatus(string(gen_buffer));
	perms_saved=TRUE;
	ShowPermSave();

}


extern "C" bool on_perm_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
	gtk_widget_hide(app_ptr->perms_window);
}


// PERMISSION checkbox callbacks
//------------------------------

// CR (content removal)  checkbox callback

extern "C" bool UL_CR(GtkButton *button, int*   data)
{
    if (settingperms) return FALSE; // avoid accidental toggle when setting initial values

    gboolean button_state;
    button_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button) );

    if (button_state)   printf("CR%d BUTTON IS ON\n",data[0]);
    else                printf("CR%d BUTTON IS OFF\n",data[0]);

	perms[data[0]].cr=button_state;
    perms_saved=FALSE;
    ShowPermSave();

}


// AU (add user) checkbox callback

extern "C" bool UL_AU(GtkButton *button, int* data)
{

	if (settingperms) return FALSE;	// avoid accidental toggle when setting initial values

    gboolean button_state;
    button_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button) );

    if (button_state)   printf("AU%d BUTTON IS ON\n",data[0]);
    else                printf("AU%d BUTTON IS OFF\n",data[0]);

	// switch on the row (eg, the user level) row is = index into perms[], eg perms[data[0]]
    perms[data[0]].au=button_state;
    perms_saved=FALSE;
    ShowPermSave();

}


extern "C" bool UL_EU(GtkButton *button, int* data)
{
    if (settingperms) return FALSE; // avoid accidental toggle when setting initial values

    gboolean button_state;
    button_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button) );

    if (button_state)   printf("EU%d BUTTON IS ON\n",data[0]);
    else                printf("EU%d BUTTON IS OFF\n",data[0]);
    perms[data[0]].eu=button_state;
    perms_saved=FALSE;
    ShowPermSave();

}

extern "C" bool UL_DEP(GtkButton *button, int* data)
{
    if (settingperms) return FALSE; // avoid accidental toggle when setting initial values
    gboolean button_state;
    button_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button) );
    perms[data[0]].dep=button_state;
    perms_saved=FALSE;
    ShowPermSave();
}

extern "C" bool UL_RPT(GtkButton *button, int* data)
{
    if (settingperms) return FALSE; // avoid accidental toggle when setting initial values
    gboolean button_state;
    button_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button) );
    perms[data[0]].rpt=button_state;
    perms_saved=FALSE;
    ShowPermSave();
}




// used as a passed arg on callbacks, gives the row number in the grid of permissions checkboxes
int arr[MAX_USER_LEVELS];


/*
	set the checkboxes with the proper values for display/edit


	GtkWidget * ulevel_lbl[MAX_USER_LEVELS];
	GtkWidget * ulevel_cr[MAX_USER_LEVELS];
	GtkWidget * ulevel_au[MAX_USER_LEVELS];
	GtkWidget * ulevel_eu[MAX_USER_LEVELS];
	GtkWidget * ulevel_dep[MAX_USER_LEVELS];
	GtkWidget * ulevel_rpt[MAX_USER_LEVELS];


struct
{
    int user_level;
    int cr;     // content removal
    int au;     // add users
    int eu;     // edit users
    int dep;    // deposits
    int rpt;    // reports
} perms[MAX_USER_LEVELS];
int perm_count;

*/
void SetPermValues(void)
{
	int index;
	settingperms=TRUE;
	// set the status of the permission checkboxes ON|OFF, in keeping with the DBF
	for (int n=0; n < user_level_count; n++)
	{
		index=GetUserLevelIndex(user_levels[n].level);	// get the index into array of this user level

printf("LEVEL:%d INDEX:%d  CR:%d  AU:%d  EU:%d  DEP:%d  RPT:%d\n",user_levels[n].level,index,perms[n].cr,perms[n].au,perms[n].eu,perms[n].dep,perms[n].rpt);


		//CR
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ulevel_cr[n]), perms[index].cr  );
		//AU
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ulevel_au[n]),  perms[index].au  );
		//EU
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ulevel_eu[n]),  perms[index].eu  );
		//DEP
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ulevel_dep[n]),  perms[index].dep  );
		//RPT
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ulevel_rpt[n]),  perms[index].rpt  );
	}


	settingperms=FALSE;
    perms_saved=FALSE;
    ShowPermSave();

}


/*
	return the index into user_levels[] array for user_level arg
*/
int GetUserLevelIndex(int user_level)
{
	for (int n=0; n<user_level_count; n++)
	{
		if (user_levels[n].level == user_level) return n;
	}
}



/*
===========================================================================================

	populate the permgrid with labels and checkboxes depending upon whats in the database

=============================================================================================
*/

void AddPermGrid(void)
{

    GetUserLevels();
	GetPerms();

	// if we already have the grid populated with checkboxes, dont do it again
	if (perm_grid_row_count == user_level_count) return;

	for (int x=0; x<MAX_USER_LEVELS; x++)
		arr[x]=x;

/*
user_level_lbl (1-x)
user_level_cr (1-x)
*/

//5 cols, 4 rows

/*
GtkWidget * ulevel_lbl[MAX_USER_LEVELS];
GtkWidget * ulevel_cr[MAX_USER_LEVELS];
GtkWidget * ulevel_au[MAX_USER_LEVELS];
GtkWidget * ulevel_eu[MAX_USER_LEVELS];
GtkWidget * ulevel_dep[MAX_USER_LEVELS];
GtkWidget * ulevel_rpt[MAX_USER_LEVELS];
*/

#define BASE 1
// one row is defined, the header

// add more rows if necessary
if ( user_level_count > BASE)
{
	for (int x=BASE; x< user_level_count; x++)
	{
		gtk_grid_insert_row (app_ptr->perms_grid,x);
	}
}

        perm_grid_row_count=user_level_count;
 
// add more columns if necessary
//gtk_grid_insert_column (app_ptr->persm_grid, gint position);


	// create the rows:: label and checkboxes
	for (int n=0; n < user_level_count; n++)
	{

//	printf("GRID %d\n",n);
		// create the ROW label (user level text)
		ulevel_lbl[n] = gtk_label_new (user_levels[n].name.c_str() );
		// attach to grid
		gtk_grid_attach(app_ptr->perms_grid, ulevel_lbl[n],0,n+1,1,1);       // user_level CR checkboxes


		// CR checkbox
		ulevel_cr[n] = gtk_check_button_new ();
		gtk_grid_attach(app_ptr->perms_grid, ulevel_cr[n],1,n+1,1,1);
		g_signal_connect (ulevel_cr[n], "clicked", G_CALLBACK (UL_CR),   &arr[n]);

		// ADD USER checkbox
	    ulevel_au[n] = gtk_check_button_new ();
	    gtk_grid_attach(app_ptr->perms_grid, ulevel_au[n],2,n+1,1,1);
	    g_signal_connect (ulevel_au[n], "toggled", G_CALLBACK (UL_AU), &arr[n]);

		// EDIT USER checkbox
	    ulevel_eu[n] = gtk_check_button_new ();
	    gtk_grid_attach(app_ptr->perms_grid, ulevel_eu[n],3,n+1,1,1);
	    g_signal_connect (ulevel_eu[n], "toggled", G_CALLBACK (UL_EU), &arr[n]);

		// DEPOSITS checkbox
	    ulevel_dep[n] = gtk_check_button_new ();
	    gtk_grid_attach(app_ptr->perms_grid, ulevel_dep[n],4,n+1,1,1);
	    g_signal_connect (ulevel_dep[n], "toggled", G_CALLBACK (UL_DEP), &arr[n]);

		// REPORTS checkbox
    	ulevel_rpt[n] = gtk_check_button_new ();
	    gtk_grid_attach(app_ptr->perms_grid, ulevel_rpt[n],5,n+1,1,1);
    	g_signal_connect (ulevel_rpt[n], "toggled", G_CALLBACK (UL_RPT), &arr[n]);


	} // end for



}






//=======================================================================
//                  END PERMS WINDOW
//=======================================================================




//=======================================================================
//                  START USER WINDOW
//=======================================================================

/*
	adding_user is used to tell the function we are adding a new user and to act appropriately

*/


void ShowUser(bool adding_user)
{
/*
	1. read DBf
	2. populate  username_txt,firstname_txt,lastname_txt,userlevel_combo,user_dept_combo,
			user_active_switch,user_id_lbl
*/

	global_adding_user=adding_user;

	// if we are adding a user, skip the search
	if (!adding_user)
		bool ret=ReadUserRecord(current_user,"");
	else
	{
		CheckUser(current_user);	// returns TRUE if username already exists
//		sprintf("gen_buffer,"User %s already exists, please choose another",username.c_str());
//		ShowStatus(gen_buffer);

	}


	// now fill in the entries in the window
	gtk_entry_set_text(GTK_ENTRY(app_ptr->username_txt),current_user_rec.username.c_str() );
    gtk_entry_set_text(GTK_ENTRY(app_ptr->firstname_txt),current_user_rec.fname.c_str() );
    gtk_entry_set_text(GTK_ENTRY(app_ptr->lastname_txt),current_user_rec.lname.c_str() );
    gtk_label_set_label(GTK_LABEL(app_ptr->lastmodified_txt),current_user_rec.lastmodified.c_str() );
    gtk_label_set_label(GTK_LABEL(app_ptr->user_id_txt),current_user_rec.id.c_str() );

	user_info_saved=TRUE;

	// setup window structures

	kb.attached = app_ptr->user_pad_target;
	kb.active = app_ptr->numpad_grid;
	kb.target= app_ptr->username_txt;
	kb.allow_alpha=TRUE;	// do not allow alpha keyboard
	kb.allow_case=TRUE;
	kb.is_upper=TRUE;		// current status of keyboard TRUE = uppercase, FALSE = lower case
	kb.abc123=KEYPAD;

	GetUserLevels();
	GetDepartments();
	PopulateUserLevelCombo();
	PopulateUserLangsCombo();
	PopulateDeptCombo();
	SetCurrentUserSelections();
	ShowUserSavedStatus();

    gtk_widget_show(app_ptr->user_window);

}







void ShowUserSavedStatus(void)
{
	string msg;

	if (user_info_saved)
        msg = getMessage(372,FALSE); // "saved"
	else
       msg = getMessage(373,FALSE); // "not saved"

	msg = "(" + msg + ")";

	gtk_label_set_label( GTK_LABEL(app_ptr->user_saved_lbl),msg.c_str() );

}


extern "C" bool on_user_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->user_window);
}

extern "C" bool on_user_cancel_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->user_window);
}

/*
	save edited user record

	save the edited user data

*/
//TODO save_btn
extern "C" bool on_user_save_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gchar *item_text = 0;   // selected item text from text combo box


	// if we are adding a new user, first check to see if the username exists in DBF, if so prompt user to enter another
	if (global_adding_user)
	{
		sprintf(gen_buffer,"User %s already exists, please choose another",current_user_rec.username.c_str());
		ShowStatus(gen_buffer);
		return FALSE;
	}

// user level
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->userlevel_combo));
	current_user_rec.user_level = string(item_text);

// lang
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->lang_combo));
	current_user_rec.lang = string(item_text);

// dept
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->user_dept_combo));
	current_user_rec.dept = string(item_text);

	const gchar *item_text2="";

// username
	item_text2 = gtk_entry_get_text(GTK_ENTRY(app_ptr->username_txt));
	current_user_rec.username = string(item_text2);

//fname
    item_text2 = gtk_entry_get_text(GTK_ENTRY(app_ptr->firstname_txt));
    current_user_rec.fname = string(item_text2);

//lname
    item_text2 =  gtk_entry_get_text(GTK_ENTRY(app_ptr->lastname_txt));
    current_user_rec.lname = string(item_text2);

//active

    gboolean active =gtk_switch_get_state (app_ptr->user_active_switch);
	if (active)
		current_user_rec.active='1';
	else
		current_user_rec.active='0';

    g_free(item_text);

    user_info_saved=TRUE;
    ShowUserSavedStatus();

	WriteUserRecord();
}



extern "C" void on_userlevel_combo_changed( GtkComboBox *widget, gpointer user_data)
{

    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->userlevel_combo));

	user_info_saved=FALSE;
	ShowUserSavedStatus();
    g_free(item_text);
}


extern "C" void on_username_txt_changed( GtkComboBox *widget, gpointer user_data)
{
    user_info_saved=FALSE;
    ShowUserSavedStatus();
}

extern "C" void on_firstname_txt_changed( GtkComboBox *widget, gpointer user_data)
{
    user_info_saved=FALSE;
    ShowUserSavedStatus();
}

extern "C" void on_lastname_txt_changed( GtkComboBox *widget, gpointer user_data)
{
    user_info_saved=FALSE;
    ShowUserSavedStatus();
}



extern "C" void on_user_dept_combo_changed( GtkComboBox *widget, gpointer user_data)
{

    gchar *item_text = 0;   // selected item text from text combo box

    // get selected item text from GtkComboBoxText object
    item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_ptr->user_dept_combo));

    user_info_saved=FALSE;
    ShowUserSavedStatus();

    g_free(item_text);
}


extern "C" void on_user_active_switch_state_set( GtkComboBox *widget, gpointer user_data)
{
    user_info_saved=FALSE;
    ShowUserSavedStatus();

}


int dept_index;
void PopulateDeptCombo(void)
{
	for (int n=0; n < dept_count; n++)
	{
		if (depts[n].department == current_user_rec.dept) dept_index=n;
		gtk_combo_box_text_append_text(app_ptr->user_dept_combo,depts[n].department.c_str() );
	}
}


int user_level_index;
void PopulateUserLevelCombo(void)
{

    for (int n=0; n < user_level_count; n++)
    {
		if (user_levels[n].level == stoi(current_user_rec.user_level)) user_level_index=n;

//		if ( user_levels[n].level != GOD_MODE)		// dont show this one
	        gtk_combo_box_text_append_text( app_ptr->userlevel_combo,user_levels[n].name.c_str()  );
    }

}

int lang_index=0;
void PopulateUserLangsCombo(void)
{

    for (int n=0; n < lang_count; n++)
    {
		if (langs[n].code == current_user_rec.lang) lang_index=n;
		if (langs[n].active == 1)
	        gtk_combo_box_text_append_text( app_ptr->lang_combo,langs[n].code.c_str()  );
    }
}


/*

	set current selection for..
	active (user_active_switch)
	lang (lang_combo)
	user_level (userlevel_combo)
	dept (user_dept_combo)

*/
void SetCurrentUserSelections(void)
{

// ACTIVE
    gboolean uactive=0;
    if (current_user_rec.active=="1") uactive=1;

    gtk_switch_set_state (app_ptr->user_active_switch, uactive);


// set lang selection
	gtk_combo_box_set_active ( GTK_COMBO_BOX(app_ptr->lang_combo),lang_index );

// set user_level selection
	gtk_combo_box_set_active (GTK_COMBO_BOX(app_ptr->userlevel_combo), user_level_index);

// set dept selection
	gtk_combo_box_set_active (GTK_COMBO_BOX(app_ptr->user_dept_combo), dept_index);




}




//=======================================================================
//                  END USER WINDOW
//=======================================================================



//=======================================================================
//                  START REPORTS WINDOW
//=======================================================================

void ShowReports(void)
{
    gtk_widget_show(app_ptr->reports_window);
}

extern "C" bool on_reports_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->reports_window);
}


extern "C" bool on_print_receipts_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("PRINT RECEIPTS\n");
}

extern "C" bool on_day_totals_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("DAY TOTALS\n");
}

extern "C" bool on_user_trans_hist_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("USER TRANS HISTORY\n");
}

extern "C" bool on_user_totals_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("USER TOTALS\n");
}

extern "C" bool on_shift_totals_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("SHIFT TOTALS\n");
}

extern "C" bool on_vend_inventory_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("VEND INVENTORY\n");
}




//=======================================================================
//                  END REPORTS WINDOW
//=======================================================================




//=======================================================================
//                  START DEPOSIT WINDOW
//=======================================================================


void ShowDeposit(void)
{
	gtk_widget_show(app_ptr->deposit_window);

}


extern "C" bool on_deposit_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->deposit_window);
}


extern "C" bool on_deposit_cash_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("DEPOSIT CASH\n");
ShowDepositCash();

}

extern "C" bool on_manual_deposit_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("MANUAL DEPOSIT\n");

}

extern "C" bool on_validator_gate_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("UNLOCK VALIDATOR GATE\n");

}

extern "C" bool on_reset_cassette_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("RESET CASSETTES\n");

}

extern "C" bool on_close_user_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("CLOSE USER\n");
}



//=======================================================================
//                  END DEPOSIT WINDOW
//=======================================================================







//=======================================================================
//					START ADMIN WINDOW
//=======================================================================

void ShowAdmin(void)
{
    gtk_widget_show(app_ptr->admin_window);
}

extern "C" bool on_admin_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->admin_window);
}

extern "C" bool on_maint_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("MAINT\n");
    ShowMaint();
}


extern "C" bool on_user_btn_clicked( GtkButton *button, AppWidgets *app)
{
	ShowUserList();
}

extern "C" bool on_eod_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_cr_btn_clicked( GtkButton *button, AppWidgets *app)
{
}

extern "C" bool on_perms_btn_clicked( GtkButton *button, AppWidgets *app)
{
	ShowPerms();
}

extern "C" bool on_load_btn_clicked( GtkButton *button, AppWidgets *app)
{
//    ShowLoad();
    printf("LOAD\n");
}

extern "C" bool on_unload_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("UNLOAD\n");
}





//=======================================================================
//					END ADMIN WINDOW
//=======================================================================



//=======================================================================
//					START STATUS WINDOW
//=======================================================================


/*
	popup the status window and display a msg
	wait for user to press OK, then put window away

*/
void ShowStatus(string status)
{
	gtk_widget_show(app_ptr->status_window);
    gtk_label_set_label(GTK_LABEL(app_ptr->status_lbl),status.c_str());

}

extern "C" bool on_status_ok_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->status_window);
}


//=======================================================================
//					END STATUS WINDOW
//=======================================================================





// show the LOAD UTD window
/*
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
*/


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


//===================================================================
//					START LOGIN SCREEN
//===================================================================

/*
	validate the user/pw in the DBF
	returns TRUE if valid, else FALSE

*/
bool ValidateUser(char * user,char * pw)
{
    char query[100];
	sprintf(query,"SELECT username,password FROM users WHERE username='%s' AND password=PASSWORD('%s');",user,pw);
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);

printf("VALIDATE:: numrows: %d\n",numrows);
	if (numrows != 0)
	{

printf("Found user: %s\n",localDBF.row[0]);
		current_user= string(localDBF.row[0]);
		current_pw = string(localDBF.row[1]);
		return TRUE;
	}

printf("USER NOT LOCATED\n");
	return FALSE;
}


#define INLEN 30
int entered_focus=0;    //0 = user_entry, 1 = pw_entry
unsigned int user_index=0;
unsigned int pw_index=0;
char entered_user[INLEN];
char entered_pw[INLEN];
char display_pw[INLEN];


// LOGIN WINDOW
void ShowLogin(void)
{
	gtk_widget_grab_focus(app_ptr->user_entry);
	entered_focus=0;
	bzero(entered_user,INLEN);
	bzero(entered_pw,INLEN);
	bzero(display_pw,INLEN);
	pw_index=0;
	user_index=0;

	// document current window properties
	kb.attached= app_ptr->login_pad_target;
	kb.active=app_ptr->numpad_grid;
	kb.target=app_ptr->user_entry;
	kb.allow_alpha=FALSE;
	kb.allow_case=FALSE;
    kb.is_upper=FALSE;	// current status of keyboard TRUE = uppercase, FALSE = lower case
    kb.abc123=KEYPAD;
	ShowCaseBtn();		// show/hide CASE btn
	ShowAbcBtn();
	InstallKB();		// reparent the kb widget into the current window


	gtk_widget_show(app_ptr->login_window);

}




/*
	send the user input data from the soft keyboard or keypad to the appropriate widget target

*/



void SetEntryText(void)
{

	// LOGIN WINDOW
	if (kb.target == app_ptr->user_entry)
	{
	    if (entered_focus==0)
			gtk_entry_set_text(GTK_ENTRY(app_ptr->user_entry),entered_user);
		else
		{
            // now mask the pw
            int pwlen = strlen(entered_pw);
            for (int n=0; n < pwlen; n++)
                display_pw[n] = '*';
   	        gtk_entry_set_text(GTK_ENTRY(app_ptr->pw_entry),display_pw);
		}
	}//endif


	// adding new user level window
	if (kb.target == app_ptr->new_userlevel_entry)
	{
		gtk_entry_set_text(GTK_ENTRY(app_ptr->new_userlevel_entry),input_text);
	}

}


/*
	store the users input to the appropriate var based on which window we are on

*/



void StoreInput(char x)
{


	// LOGIN WINDOW
    if (kb.target == app_ptr->user_entry)
    {
		if (entered_focus==0)
			entered_user[user_index++]=x;
		else
			entered_pw[pw_index++]=x;

	}

	// adding new user level window
	if (kb.target == app_ptr->new_userlevel_entry)
	{
 		if ( kb.is_upper)	// current status of keyboard TRUE = uppercase, FALSE = lower case
		{
			input_text[input_text_index++]= x;
			ucase(input_text);
		}
		else
	 		input_text[input_text_index++]=x;
	}

}




extern "C" bool on_abc_btn_clicked( GtkButton *button, AppWidgets *app)
{
	string msg;

	kb.abc123 ^= 1;
        // REPARENT(old parent, new parent, widget)


	if (kb.abc123==0)
	{
		// NUMPAD SHOWS
		kb.active=app_ptr->numpad_grid;
	    msg = getMessage(370,FALSE); // "ABC"
    	gtk_button_set_label( GTK_BUTTON(app_ptr->abc_btn),msg.c_str() );

		// put the keyboard back home->keyboard_window
        REPARENT(app_ptr->login_pad_target,app_ptr->keyboard_window,app_ptr->keyboard_grid)

		// put the numpad on the login screen
		REPARENT(app_ptr->numpad_window,app_ptr->login_pad_target,app_ptr->numpad_grid)
	}
	else
	{
		// KEYBOARD SHOWS
		kb.active=app_ptr->keyboard_grid;
        msg = getMessage(371,FALSE); // "123"
        gtk_button_set_label( GTK_BUTTON(app_ptr->abc_btn),msg.c_str() );

		// put the numpad back homw->numpad_window
        REPARENT(app_ptr->login_pad_target,app_ptr->numpad_window,app_ptr->numpad_grid)

		// put the keypad on the login screen
        REPARENT(app_ptr->keyboard_window,app_ptr->login_pad_target,app_ptr->keyboard_grid)
	}


}


extern "C" bool on_one_btn_clicked( GtkButton *button, AppWidgets *app)
{
	StoreInput('1');
	SetEntryText();
	printf("ONE\n");
}

extern "C" bool on_two_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('2');
    SetEntryText();
    printf("TWO\n");
}

extern "C" bool on_three_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('3');
    SetEntryText();
    printf("THREE\n");
}

extern "C" bool on_four_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('4');
    SetEntryText();
    printf("FOUR\n");
}

extern "C" bool on_five_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('5');
    SetEntryText();
    printf("FIVE\n");
}

extern "C" bool on_six_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('6');
    SetEntryText();
    printf("SIX\n");
}

extern "C" bool on_seven_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('7');
    SetEntryText();
    printf("SEVEN\n");
}

extern "C" bool on_eight_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('8');
    SetEntryText();
    printf("EIGHT\n");
}


extern "C" bool on_nine_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('9');
    SetEntryText();
    printf("NINE\n");

}

extern "C" bool on_zero_btn_clicked( GtkButton *button, AppWidgets *app)
{
    StoreInput('0');
    SetEntryText();
    printf("ZERO\n");
}

extern "C" bool on_tab_btn_clicked( GtkButton *button, AppWidgets *app)
{
	TabButton();

}

extern "C" bool on_bksp_btn_clicked( GtkButton *button, AppWidgets *app)
{
	BackSpace();

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
    printf("HEBREW btn\n");
}

extern "C" bool on_login_btn_clicked( GtkButton *button, AppWidgets *app)
{
	string msg;
	bool ret= ValidateUser(entered_user,entered_pw);


	if (ret)
	{
		// LOGIN SUCCESS
		logged_in=TRUE;
	    gtk_widget_hide(app_ptr->login_window);
	    ReHomeKB();
		ShowMainMenu();
	}
	else
	{
		// LOGIN FAILED
		logged_in=FALSE;
		entered_focus=0;
		user_index=0;
		pw_index=0;
		bzero(entered_user,INLEN);
		bzero(entered_pw,INLEN);
		bzero(display_pw,INLEN);
        entered_focus=0;
	    SetEntryText();
        entered_focus=1;
        SetEntryText();
		entered_focus=0;
		sprintf(gen_buffer,"LOGIN ERROR: username: %s   pw: %s",entered_user,entered_pw);
		WriteSystemLog(gen_buffer);
		msg=getMessage(121,FALSE);
        ShowStatus("Error: invalid username or password");
	}

}


extern "C" bool on_login_back_btn_clicked( GtkButton *button, AppWidgets *app)
{
    ReHomeKB();
    gtk_widget_hide(app_ptr->login_window);
    ShowSplashWindow();
}



//===================================================================
//                  END LOGIN SCREEN
//===================================================================



//===================================================================
//                  START KEYBOAD BUTTONS
//===================================================================


void TabButton(void)
{

	// LOGIN
	if (kb.attached == app_ptr->login_pad_target)
	{
        //user_entry
        // pw_entry
        entered_focus ^=1;  // toggle focus

        if (entered_focus == 0)
            gtk_widget_grab_focus(app_ptr->user_entry);
        else
            gtk_widget_grab_focus(app_ptr->pw_entry);
	}

	// ADD/EDIT USER
	if (kb.attached == app_ptr->user_pad_target)
	{
	}

    // ADD USER LEVEL
    if (kb.attached == app_ptr->perm_kb_target)
    {
    }



    printf("TAB\n");
}



void BackSpace(void)
{

    if (kb.attached == app_ptr->login_pad_target)
    {

	    if (entered_focus==0)
	    {
	        if (user_index==0) return;
	        entered_user[--user_index]=0;
	    }
	    else
	    {
	        if (pw_index==0) return;
	        entered_pw[--pw_index]=0;
	    }
	    SetEntryText();
	    printf("BKSP\n");

	}

    // ADD/EDIT USER
    if (kb.attached == app_ptr->user_pad_target)
    {
    }

    // ADD USER LEVEL
    if (kb.attached == app_ptr->perm_kb_target)
    {
		input_text[--input_text_index]=0;
    }


}




extern "C" bool on_kb_bksp_btn_clicked( GtkButton *button, AppWidgets *app) { BackSpace(); SetEntryText();}
extern "C" bool on_case_btn_clicked( GtkButton *button, AppWidgets *app) { kb.is_upper ^=1; SetKeyboardCase();}
extern "C" bool on_kbrd_tab_btn_clicked( GtkButton *button, AppWidgets *app) {TabButton();}

/*
    StoreInput('8');
    SetEntryText();
*/

//    StoreInput('0');    SetEntryText();


extern "C" bool on_a_btn_clicked( GtkButton *button, AppWidgets *app) { printf("A\n"); StoreInput('a');    SetEntryText();}
extern "C" bool on_b_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('b');    SetEntryText();}
extern "C" bool on_c_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('c');    SetEntryText();}
extern "C" bool on_d_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('d');    SetEntryText();}
extern "C" bool on_e_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('e');    SetEntryText();}
extern "C" bool on_f_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('f');    SetEntryText();}
extern "C" bool on_g_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('g');    SetEntryText();}
extern "C" bool on_h_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('h');    SetEntryText();}
extern "C" bool on_i_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('i');    SetEntryText();}
extern "C" bool on_j_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('j');    SetEntryText();}
extern "C" bool on_k_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('k');    SetEntryText();}
extern "C" bool on_l_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('l');    SetEntryText();}
extern "C" bool on_m_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('m');    SetEntryText();}
extern "C" bool on_n_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('n');    SetEntryText();}
extern "C" bool on_o_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('o');    SetEntryText();}
extern "C" bool on_p_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('p');    SetEntryText();}
extern "C" bool on_q_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('q');    SetEntryText();}
extern "C" bool on_r_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('r');    SetEntryText();}
extern "C" bool on_s_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('s');    SetEntryText();}
extern "C" bool on_t_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('t');    SetEntryText();}
extern "C" bool on_u_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('u');    SetEntryText();}
extern "C" bool on_v_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('v');    SetEntryText();}
extern "C" bool on_w_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('w');    SetEntryText();}
extern "C" bool on_x_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('x');    SetEntryText();}
extern "C" bool on_y_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('y');    SetEntryText();}
extern "C" bool on_z_btn_clicked( GtkButton *button, AppWidgets *app) {StoreInput('z');    SetEntryText();}



//===================================================================
//                  END KEYBOAD BUTTONS
//===================================================================




//===================================================================
//                  START SPLASH SCREEN
//===================================================================



void ShowSplashWindow(void)
{
    string msg;
    gtk_widget_show(app_ptr->splash_window);
}



extern "C" bool on_login_exit_btn_clicked( GtkButton *button, AppWidgets *app)
{
//    gtk_widget_hide(app_ptr->splash_window);
//	gtk_widget_destroy( app_ptr->splash_window );
    gtk_main_quit();

}


extern "C" bool on_touch_here_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->splash_window);
	ShowLogin();
}


extern "C" bool on_verify_note_btn_clicked( GtkButton *button, AppWidgets *app)
{
//	mei_verify_bill_func() ;
	printf("VERIFY NOTE\n");
}

//===================================================================
//                  END SPLASH SCREEN
//===================================================================








//===================================================================
//				START MAINT WINDOW
//===================================================================


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

//===================================================================
//				END MAINT WINDOW
//===================================================================




//===================================================================
//			START MEI MAINT WINDOW
//===================================================================

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
	Validator_reset(0);
}


extern "C" bool on_mei_stack_btn_clicked( GtkButton *button, AppWidgets *app)
{
	Validator_stack(0);
}


extern "C" bool on_mei_inventory_btn_clicked( GtkButton *button, AppWidgets *app)
{
	Validator_info(0);
}


//===================================================================
//				END MEI MAINT WINDOW
//===================================================================




//===================================================================
//				START UTD MAINT WINDOW
//===================================================================


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


//===================================================================
//				END UTD MAINT WINDOW
//===================================================================







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
//	gtk_widget_show_all(app_ptr->main_menu);
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


struct
{
	string fieldname;
} permfields[10];
int perm_field_count;

/*
	get a list of fieldnames from the perms table

*/
void GetPermissionFields(void)
{

	char query[] = "select `COLUMN_NAME` FROM `INFORMATION_SCHEMA`.`COLUMNS` WHERE `TABLE_SCHEMA` = 'fking' AND `TABLE_NAME` = 'perms';";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer
	perm_field_count=numrows;

	for (int n=0; n < numrows; n++)
	{
		printf("FIELD: %s\n",localDBF.row[0]);
		permfields[n].fieldname = string(localDBF.row[0]);
		GetRow(&localDBF);
	}

}




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

	API_Handler();

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


	MEIpoll();		// in USB_gateway.cpp

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
	gtk_window_set_default_size(GTK_WINDOW(app_ptr->main_menu), screenres.horiz, screenres.vert);

    gtk_window_set_default_size(GTK_WINDOW(app_ptr->lockconfig_window), screenres.horiz, screenres.vert);
//    gtk_window_set_default_size(GTK_WINDOW(app_ptr->load_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->maint_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->utd_maint_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->mei_maint_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->login_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->splash_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->admin_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->user_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->settings_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->userlist_window), screenres.horiz, screenres.vert);

    gtk_window_set_default_size(GTK_WINDOW(app_ptr->perms_window), screenres.horiz, screenres.vert);

    gtk_window_set_default_size(GTK_WINDOW(app_ptr->add_user_level_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->deposit_window), screenres.horiz, screenres.vert);
    gtk_window_set_default_size(GTK_WINDOW(app_ptr->reports_window), screenres.horiz, screenres.vert);

    gtk_window_set_default_size(GTK_WINDOW(app_ptr->deposit_cash_window), screenres.horiz, screenres.vert);

}


/*
	show the CASE btn on the keyboard, or not depending upon value of kb.allow_case

*/

void ShowCaseBtn(void)
{

    if (kb.attached == app_ptr->login_pad_target)
	{

		if (kb.allow_case)
		{
		    gtk_widget_show(app_ptr->case_btn);
			kb.is_upper=TRUE;
			SetKeyboardCase();
		}
		else
		{
		    gtk_widget_hide(app_ptr->case_btn);
			kb.is_upper=FALSE;
		}

	}

    if (kb.attached == app_ptr->perm_kb_target)
	{
	}


}



void ShowAbcBtn(void)
{
    if (kb.attached == app_ptr->login_pad_target)
    {
		if (kb.allow_alpha)
	    {
			// allow alpha and numeric
	        gtk_widget_show(app_ptr->abc_btn);
	        kb.is_upper=TRUE;
	    }
	    else
	        gtk_widget_hide(app_ptr->abc_btn);

	}

    if (kb.attached == app_ptr->perm_kb_target)
    {
    }

}



void SetKeyboardCase(void)
{
	string msg;
	int x;

	if (kb.is_upper)
		x=400;		// UCASE chars
	else
		x=430;		// LCASE chars

	    msg = getMessage(x++,FALSE); // "A"
	    gtk_button_set_label( GTK_BUTTON(app_ptr->a_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "B"
        gtk_button_set_label( GTK_BUTTON(app_ptr->b_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "C"
        gtk_button_set_label( GTK_BUTTON(app_ptr->c_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "D"
        gtk_button_set_label( GTK_BUTTON(app_ptr->d_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "E"
        gtk_button_set_label( GTK_BUTTON(app_ptr->e_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "F"
        gtk_button_set_label( GTK_BUTTON(app_ptr->f_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "G"
        gtk_button_set_label( GTK_BUTTON(app_ptr->g_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "H"
        gtk_button_set_label( GTK_BUTTON(app_ptr->h_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "I"
        gtk_button_set_label( GTK_BUTTON(app_ptr->i_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "J"
        gtk_button_set_label( GTK_BUTTON(app_ptr->j_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "K"
        gtk_button_set_label( GTK_BUTTON(app_ptr->k_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "L"
        gtk_button_set_label( GTK_BUTTON(app_ptr->l_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "M"
        gtk_button_set_label( GTK_BUTTON(app_ptr->m_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "N"
        gtk_button_set_label( GTK_BUTTON(app_ptr->n_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "O"
        gtk_button_set_label( GTK_BUTTON(app_ptr->o_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "P"
        gtk_button_set_label( GTK_BUTTON(app_ptr->p_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "Q"
        gtk_button_set_label( GTK_BUTTON(app_ptr->q_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "R"
        gtk_button_set_label( GTK_BUTTON(app_ptr->r_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "S"
        gtk_button_set_label( GTK_BUTTON(app_ptr->s_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "T"
        gtk_button_set_label( GTK_BUTTON(app_ptr->t_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "U"
        gtk_button_set_label( GTK_BUTTON(app_ptr->u_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "V"
        gtk_button_set_label( GTK_BUTTON(app_ptr->v_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "W"
        gtk_button_set_label( GTK_BUTTON(app_ptr->w_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "X"
        gtk_button_set_label( GTK_BUTTON(app_ptr->x_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "Y"
        gtk_button_set_label( GTK_BUTTON(app_ptr->y_btn),msg.c_str() );
        msg = getMessage(x++,FALSE); // "Z"
        gtk_button_set_label( GTK_BUTTON(app_ptr->z_btn),msg.c_str() );



}



/*
	set all the labels, buttons etc text from the language file
*/

void SetLabels(void)
{
	string msg;

	SetKeyboardCase();

// status popup
    msg = getMessage(48,FALSE); // "OK"
    gtk_button_set_label( GTK_BUTTON(app_ptr->status_ok_btn),msg.c_str() );


// splash window
    msg = getMessage(87,FALSE); // "VERIFY NOTE"
    gtk_button_set_label( GTK_BUTTON(app_ptr->verify_note_btn),msg.c_str() );


// login window
    msg = getMessage(80,FALSE); // "ENGLISH"
    gtk_button_set_label( GTK_BUTTON(app_ptr->en_btn),msg.c_str() );

    msg = getMessage(81,FALSE); // "ESPANOL"
    gtk_button_set_label( GTK_BUTTON(app_ptr->es_btn),msg.c_str() );

    msg = getMessage(82,FALSE); // "FRANCAIS"
    gtk_button_set_label( GTK_BUTTON(app_ptr->fr_btn),msg.c_str() );

    msg = getMessage(83,FALSE); // "HEBREW"
    gtk_button_set_label( GTK_BUTTON(app_ptr->he_btn),msg.c_str() );

    msg = getMessage(84,FALSE); // "LOGIN"
    gtk_button_set_label( GTK_BUTTON(app_ptr->login_btn),msg.c_str() );

    msg = getMessage(86,FALSE); // "EXIT"
    gtk_button_set_label( GTK_BUTTON(app_ptr->login_exit_btn),msg.c_str() );




// main window

    msg = getMessage(85,FALSE); // "LOGOUT"
    gtk_button_set_label( GTK_BUTTON(app_ptr->logout_btn),msg.c_str() );

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

    msg = getMessage(254,FALSE); // "ADMIN"
    gtk_button_set_label( GTK_BUTTON(app_ptr->admin_btn),msg.c_str() );


    msg = getMessage(256,FALSE); // "DEPOSIT"
    gtk_button_set_label( GTK_BUTTON(app_ptr->deposit_btn),msg.c_str() );

    msg = getMessage(257,FALSE); // "REPORTS"
    gtk_button_set_label( GTK_BUTTON(app_ptr->reports_btn),msg.c_str() );

    msg = getMessage(258,FALSE); // "WITHDRAWL"
    gtk_button_set_label( GTK_BUTTON(app_ptr->withdrawl_btn),msg.c_str() );





// admin window

    msg = getMessage(50,FALSE); // "CLOSE"
    gtk_button_set_label( GTK_BUTTON(app_ptr->admin_close_btn),msg.c_str() );

    msg = getMessage(300,FALSE); // "USERS"
    gtk_button_set_label( GTK_BUTTON(app_ptr->user_btn),msg.c_str() );


// user window

    msg = getMessage(50,FALSE); // "CLOSE"
    gtk_button_set_label( GTK_BUTTON(app_ptr->user_close_btn),msg.c_str() );

    msg = getMessage(350,FALSE); // "User Name"
    gtk_label_set_label( GTK_LABEL(app_ptr->username_lbl),msg.c_str() );

    msg = getMessage(351,FALSE); // "First Name"
    gtk_label_set_label( GTK_LABEL(app_ptr->firstname_lbl),msg.c_str() );

    msg = getMessage(352,FALSE); // "Last Name"
    gtk_label_set_label( GTK_LABEL(app_ptr->lastname_lbl),msg.c_str() );

    msg = getMessage(353,FALSE); // "User Level"
    gtk_label_set_label( GTK_LABEL(app_ptr->userlevel_lbl),msg.c_str() );

    msg = getMessage(354,FALSE); // "Dept"
    gtk_label_set_label( GTK_LABEL(app_ptr->dept_lbl),msg.c_str() );

    msg = getMessage(355,FALSE); // "Active"
    gtk_label_set_label( GTK_LABEL(app_ptr->useractive_lbl),msg.c_str() );

    msg = getMessage(356,FALSE); // "User ID"
    gtk_label_set_label( GTK_LABEL(app_ptr->user_id_lbl),msg.c_str() );

    msg = getMessage(357,FALSE); // "SAVE"
    gtk_button_set_label( GTK_BUTTON(app_ptr->user_save_btn),msg.c_str() );



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

    msg=getMessage(227,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->mei_stack_btn),msg.c_str() );

    msg=getMessage(228,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->mei_inventory_btn),msg.c_str() );



    msg=getMessage(50,FALSE);
    gtk_button_set_label(GTK_BUTTON(app_ptr->mei_close_btn),msg.c_str() );



// deposit window

    msg=getMessage(50,FALSE);	// "CLOSE"
    gtk_button_set_label(GTK_BUTTON(app_ptr->deposit_close_btn),msg.c_str() );

    msg=getMessage(130,FALSE);	// "DEPOSIT CASH"
    gtk_button_set_label(GTK_BUTTON(app_ptr->deposit_cash_btn),msg.c_str() );

    msg=getMessage(131,FALSE);	// "UNLOCK VALIDATOR GATE"
    gtk_button_set_label(GTK_BUTTON(app_ptr->validator_gate_btn),msg.c_str() );

    msg=getMessage(132,FALSE);   // "CLOSE USER"
    gtk_button_set_label(GTK_BUTTON(app_ptr->close_user_btn),msg.c_str() );

    msg=getMessage(133,FALSE);   // "MANUAL DEPOSIT"
    gtk_button_set_label(GTK_BUTTON(app_ptr->manual_deposit_btn),msg.c_str() );

    msg=getMessage(134,FALSE);   // "RESET CASSETTES"
    gtk_button_set_label(GTK_BUTTON(app_ptr->reset_cassette_btn),msg.c_str() );


    msg=getMessage(135,FALSE);   // "DEPOSITS MENU"
    gtk_label_set_label(GTK_LABEL(app_ptr->deposits_title),msg.c_str() );



// reports window


    msg=getMessage(50,FALSE);   // "CLOSE"
    gtk_button_set_label(GTK_BUTTON(app_ptr->reports_close_btn),msg.c_str() );

    msg=getMessage(140,FALSE);   // "REPRINT RECEIPTS"
    gtk_button_set_label(GTK_BUTTON(app_ptr->print_receipts_btn),msg.c_str() );

    msg=getMessage(141,FALSE);   // "DAY TOTALS"
    gtk_button_set_label(GTK_BUTTON(app_ptr->day_totals_btn),msg.c_str() );

    msg=getMessage(142,FALSE);   // "USER TRANS HIST"
    gtk_button_set_label(GTK_BUTTON(app_ptr->user_trans_hist_btn),msg.c_str() );

    msg=getMessage(143,FALSE);   // "USER TOTALS"
    gtk_button_set_label(GTK_BUTTON(app_ptr->user_totals_btn),msg.c_str() );


    msg=getMessage(144,FALSE);   // "SHIFT TOTALS"
    gtk_button_set_label(GTK_BUTTON(app_ptr->shift_totals_btn),msg.c_str() );

    msg=getMessage(145,FALSE);   // "VEND INVENTORY"
    gtk_button_set_label(GTK_BUTTON(app_ptr->vend_inventory_btn),msg.c_str() );

    msg=getMessage(146,FALSE);   // "REPORTS MENU"
    gtk_label_set_label(GTK_LABEL(app_ptr->reports_title),msg.c_str() );


	// add user level window
    msg = getMessage(360,FALSE); // "ADD USER LEVEL"
    gtk_label_set_label( GTK_LABEL(app_ptr->add_userlevel_title),msg.c_str() );


    msg = getMessage(361,FALSE); // "NEW USER LEVEL NAME"
    gtk_label_set_label( GTK_LABEL(app_ptr->new_userlevel_prompt),msg.c_str() );

    msg=getMessage(49,FALSE);   // "CANCEL"
    gtk_button_set_label(GTK_BUTTON(app_ptr->add_userlevel_cancel_btn),msg.c_str() );


	// deposit cash window
    msg = getMessage(150,FALSE); // "DEPOSIT CASH"
    gtk_label_set_label( GTK_LABEL(app_ptr->deposit_cash_title),msg.c_str() );


    msg=getMessage(151,FALSE);   // "DEPOSIT COMPLETE"
    gtk_button_set_label(GTK_BUTTON(app_ptr->deposit_complete_btn),msg.c_str() );

    msg=getMessage(152,FALSE);   // "INSERT CASH ONE BILL AT A TIME.TOUCH DEPOSIT COMPLETE WHEN FINISHED."
    gtk_label_set_label(GTK_LABEL(app_ptr->deposit_cash_lbl),msg.c_str() );



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
    gtk_window_set_title( GTK_WINDOW(app_ptr->main_menu), gen_buffer );

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

/*
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

*/



// MAIN WINDOW
void ShowMainMenu(void)
{
    gtk_widget_show_all(app_ptr->main_menu);
}

extern "C" bool on_admin_btn_clicked( GtkButton *button, AppWidgets *app)
{
	ShowAdmin();
    printf("ADMIN\n");
}


extern "C" bool on_settings_btn_clicked( GtkButton *button, AppWidgets *app)
{
	ShowSettings();
}


extern "C" bool on_deposit_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("DEPOSIT\n");
	ShowDeposit();
}


extern "C" bool on_withdrawl_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("WITHDRAWL\n");
}

extern "C" bool on_reports_btn_clicked( GtkButton *button, AppWidgets *app)
{
printf("REPORTS\n");
	ShowReports();
}






extern "C" bool on_lock_btn_clicked( GtkButton *button, AppWidgets *app)
{
    printf("calling unlock_lock\n");
    Unlock_Lock(0);     // in usb_gateway.cpp
    printf("DOORS\n");
}

extern "C" bool on_logout_btn_clicked( GtkButton *button, AppWidgets *app)
{
	logged_in=FALSE;
    gtk_widget_hide(app_ptr->login_window);
	ShowSplashWindow();
}



//=========================================================
//              START DEPOSIT CASH
//=========================================================

void ShowDepositCash(void)
{
    gtk_widget_show_all(app_ptr->deposit_cash_window);
}

extern "C" bool on_deposit_complete_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->deposit_cash_window);

}


/*
    add a deposit listitem to the listbox (deposit_cash_listbox)
*/

void AddDeposit(char *txt)
{
	AddListItem(app_ptr->deposit_cash_listbox,txt);
}



//=========================================================
//              END DEPOSIT CASH
//=========================================================





//=========================================================
//				START USER LIST
//=========================================================

void ShowUserList(void)
{

	GetAllUsers();

printf("SHOWUSERLIST\n");

	for (int n=0; n <user_count; n++)
	{
		sprintf(gen_buffer,"%s %s %s %s",users[n].username.c_str(), users[n].fname.c_str(), users[n].lname.c_str(), users[n].user_level.c_str()  );
		AddListItem(app_ptr->user_listbox,gen_buffer );
	}



    gtk_widget_show_all(app_ptr->userlist_window);
}




extern "C" bool on_userlist_close_btn_clicked( GtkButton *button, AppWidgets *app)
{
    gtk_widget_hide(app_ptr->userlist_window);
}



/*
    Get the selected row from user_listbox and return the text

    the listbox is arranged as follows...

    GTK+3 only

    Gtk_listbox
    |____Gtk_listboxrow
        |_____ Gtk_box
                |_______Gtk_label (which is where our text resides

    to retrieve the text, first get the selected row pointer
    then get a list of the children (there is only one, a box)
    get that child
    then get its children, there is only one, a label
    then get the text from the label

*/

const char* GetRowText(void)
{
    // we havea GtkListBoxRow --> GtkBox --> GtkLabel

    // get the selected row
    GtkListBoxRow * selrow= gtk_list_box_get_selected_row ((GtkListBox *)app_ptr->user_listbox);

    // get the children of the selected row (a box)
    GList *children = gtk_container_get_children(GTK_CONTAINER(selrow));
    // get the box connected to the above row
    GtkWidget *childbox = GTK_WIDGET(children->data);   // get children of the row, eg a box

    // get the children of the above box (a label)
    GList *childlbl = gtk_container_get_children(GTK_CONTAINER(childbox));

    // get the data from the label
    const gchar * txt = gtk_label_get_text(GTK_LABEL(childlbl->data) );


    gtk_label_set_label( GTK_LABEL(app_ptr->seluser_lbl),txt );


    // be a good neighbor
    g_list_free(children);
    g_list_free(childlbl);

    return txt;
}


extern "C" bool on_user_listbox_row_selected( GtkButton *button, AppWidgets *app)
{
    const char * txt = GetRowText();

    vector <string> usr=split(txt," ");     // usr[0] = username usr[1] = fname usr[2] = lname


    printf("===============================\n");
    printf("SELECTED::: %s\n",txt);
    printf("===============================\n");

}






extern "C" bool on_user_up_btn_clicked( GtkButton *button, AppWidgets *app)
{
    GtkListBoxRow * row;

    // get current row
    row= gtk_list_box_get_selected_row ((GtkListBox *)app_ptr->user_listbox);


    // get current row's index
    gint index = gtk_list_box_row_get_index (row);
//printf("UP INDEX: %d\n",index);

    if (index >0)
        index--;    // move to prev row
    else
        return TRUE;

    // get the row at the new index
    row = gtk_list_box_get_row_at_index ( (GtkListBox *)app_ptr->user_listbox,  index);

    // now select the new row
    gtk_list_box_select_row ((GtkListBox *)app_ptr->user_listbox,row);


}




extern "C" bool on_user_dn_btn_clicked( GtkButton *button, AppWidgets *app)
{
    GtkListBoxRow * row;

    // get current row
    row= gtk_list_box_get_selected_row ((GtkListBox *)app_ptr->user_listbox);

// NOT NEEDED, DOES THIS AUTOMAGICALLY
// un-select current row
//  gtk_list_box_unselect_row ((GtkListBox *)app_ptr->user_listbox,  row);

    // get current row's index
    gint index = gtk_list_box_row_get_index (row);
//printf("DOWN INDEX: %d\n",index);

    if (index < user_count-1)
        index++;    // move to next row
    else
        return TRUE;

    // get the row at the new index
    row = gtk_list_box_get_row_at_index ( (GtkListBox *)app_ptr->user_listbox,  index);

    // now select the new row
    gtk_list_box_select_row ((GtkListBox *)app_ptr->user_listbox,row);

}


/*
	edit the current user
*/

extern "C" bool on_user_edit_btn_clicked( GtkButton *button, AppWidgets *app)
{
    const char * txt = GetRowText();

    vector <string> usr=split(txt," ");     // usr[0] = username usr[1] = fname usr[2] = lname

    // set curent_user before calling
	current_user=usr[0];
    ShowUser(FALSE);

}



/*
//TODO    add a new user

*/
extern "C" bool on_user_add_btn_clicked( GtkButton *button, AppWidgets *app)
{

/*
	1. empty current_user_rec
	2. set user_level to lowest level
	3. set dept to default
	4. set lang to 'en'
	5. call ShowUser(); (show user will call ReadUser, make sure it handles this properly, likely by the flag below)
	6. set a flag such that the SAVE button knows to create a new record
*/

	current_user_rec.username="";
	current_user_rec.password="";
	current_user_rec.lang="en";
	current_user_rec.user_level="10";
	current_user_rec.dept="";
	current_user_rec.active="1";

	ShowUser(TRUE);
}


/*
    delete displayed user
*/
extern "C" bool on_user_del_btn_clicked( GtkButton *button, AppWidgets *app)
{
    const char * txt = GetRowText();

    vector <string> usr=split(txt," ");     // usr[0] = username usr[1] = fname usr[2] = lname

    // set curent_user before calling
    current_user=usr[0];

    char query[200];
    sprintf(query,"DELETE FROM users WHERE username='%s';",usr[0].c_str() );


    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);

    if (numrows !=0)
	{
		sprintf(gen_buffer,"DELETED user %s",usr[0].c_str());
		ShowStatus(string(gen_buffer));
	}
	else
	{
        sprintf(gen_buffer,"ERROR deleting user %s",usr[0].c_str());
 		ShowStatus(string(gen_buffer));
	}

	WriteSystemLog(gen_buffer);

}



//=========================================================
//              END USER LIST
//=========================================================




//=========================================================
//				START LISTBOX FUNCTIONS
//=========================================================

/*
	taken from...
	https://github.com/linuxmint/gtk/blob/master/tests/testlist3.c


	to populate the listbox with items, use the following

	example call:
    char str[]="gary";
    AddListItem(app_ptr->user_listbox,str);



	NOTE: after populating the list items, you must use show_all for everything to display, as...
    gtk_widget_show_all(app_ptr->userlist_window);


*/


// add the text to a listbox
void AddListItem(GtkWidget * list, char *txt)
{
    GtkWidget *row;
    row = create_row(txt);
    gtk_list_box_insert(GTK_LIST_BOX(list),row,-1);



}


// create a row for a listbox
static GtkWidget * create_row(const gchar *txt)
{
    GtkWidget *row, *box, *label;

    row = gtk_list_box_row_new();
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    label = gtk_label_new(txt);
    gtk_container_add(GTK_CONTAINER(row),box);
    gtk_container_add(GTK_CONTAINER(box),label);
    return row;
}


//=========================================================
//              END LISTBOX FUNCTIONS
//=========================================================


/*
	get a count of all users in the database

*/

int GetUserCount(void)
{
    char query[200];
    sprintf(query,"SELECT id FROM users");

    int result =  QueryDBF(&localDBF,query);
	user_count = GetRow(&localDBF);

}



/*
	read all active users in the database and populate users array of structs

*/

void GetAllUsers(void)
{
	GetUserCount();

	users = new user_rec[user_count];

	char query[200];
	sprintf(query,"SELECT username,lang,fname,lname,user_level,dept,created,lastmodified,active,id FROM users WHERE active='1'");

	int result =  QueryDBF(&localDBF,query);
	int numrows = GetRow(&localDBF);

    if (numrows !=0)
    {
		for (int n=0; n < numrows; n++)
		{
			users[n].username = string(localDBF.row[0]);
			users[n].lang = string(localDBF.row[1]);
			users[n].fname =  string(localDBF.row[2]);
			users[n].lname = string(localDBF.row[3]);
			users[n].user_level = localDBF.row[4];
			users[n].dept = string(localDBF.row[5]);
            users[n].created = string(localDBF.row[6]);
			users[n].lastmodified = string(localDBF.row[7]);
			users[n].active = localDBF.row[8];
			users[n].id = localDBF.row[9];

//       printf("User: %s  %s  %s\n",users[n].username.c_str(), users[n].fname.c_str(), users[n].lname.c_str() );
			GetRow(&localDBF);  // get next row
		}


//	int numusers = sizeof(users)/sizeof(users[0]);
//	printf("Users (calc): %d   returned: %d\n",numusers , numrows);
	}

	return;
}


/*
	read the entire xml/config.xml file into a string and return it

*/

string configfile(2000,0);

string SerializeConfig(void)
{
	configfile = ReadFile(config_file);
	return configfile;
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
            pelem = elem->FirstChildElement("validator_left");
			cfg.validator_left=FALSE;
            if (pelem)
			{
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
                if (strcmp(gen_buffer,"enabled") == 0)
                    cfg.validator_left=TRUE;
			}


            pelem = elem->FirstChildElement("validator_right");
            cfg.validator_right=FALSE;
            if (pelem)
            {
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
                if (strcmp(gen_buffer,"enabled") == 0)
                    cfg.validator_right=TRUE;
            }

            pelem = elem->FirstChildElement("validator_ucd");
            cfg.validator_ucd=FALSE;
            if (pelem)
            {
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
                if (strcmp(gen_buffer,"enabled") == 0)
                    cfg.validator_ucd=TRUE;
            }




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
            cfg.utd=FALSE;
            if (pelem)
            {
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
                if (strcmp(gen_buffer,"enabled") == 0)
                    cfg.utd=TRUE;
            }


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


/*
            pelem = elem->FirstChildElement("api");
            if (pelem)
                strcpy(cfg.api, (char*) pelem->GetText());
            else
                strcpy(cfg.api, (char*) "disabled");
*/


            pelem = elem->FirstChildElement("pelicano");
            if (pelem)
			{
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
				if (strcmp(gen_buffer,"enabled") == 0)
					cfg.pelicano=TRUE;
				else
					cfg.pelicano=FALSE;
			}
			else
				cfg.pelicano=FALSE;

            pelem = elem->FirstChildElement("gsr50");
            if (pelem)
            {
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
                if (strcmp(gen_buffer,"enabled") == 0)
                    cfg.gsr50=TRUE;
                else
                    cfg.gsr50=FALSE;
            }
            else
                cfg.gsr50=FALSE;


/*
            pelem = elem->FirstChildElement("recycler");
            if (pelem)
                strcpy(cfg.recycler, (char*) pelem->GetText());
            else
                strcpy(cfg.recycler, (char*) "disabled");
*/

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

			pelem = elem->FirstChildElement("fileenable");
			if (pelem)
				strcpy(cfg.logenabled, (char*) pelem->GetText());
			else
				strcpy(cfg.logenabled,  (char*)"disabled");

            pelem = elem->FirstChildElement("logfile");
            if (pelem)
                strcpy(cfg.logfile, (char*) pelem->GetText());
            else
                strcpy(cfg.logfile,  (char*)"/var/log/vsafe.log");

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



        if(elemName == "remoteserver")
        {
            pelem = elem->FirstChildElement("enabled");
			if (pelem)
            {
                strcpy(gen_buffer,(char *)pelem->GetText());
                lcase(gen_buffer);
                if (strcmp(gen_buffer,"yes") == 0)
                    cfg.remoteserver=TRUE;
                else
                    cfg.remoteserver=FALSE;
            }
            else
                cfg.remoteserver=FALSE;




            pelem = elem->FirstChildElement("ip");
            if (pelem)
                strcpy(cfg.cloud_ip, (char*) pelem->GetText());
            else
                strcpy(cfg.cloud_ip,  (char*)"127.0.0.1");

           pelem = elem->FirstChildElement("port");
            if (pelem)
                strcpy(cfg.cloud_port, (char*) pelem->GetText());
            else
                strcpy(cfg.cloud_port,  (char*)"1234");


		}

        if (elemName == "timeouts")
        {
           pelem = elem->FirstChildElement("validator");
            if (pelem)
                strcpy(cfg.validator_timeout, (char*) pelem->GetText());
            else
                strcpy(cfg.validator_timeout, (char*) "30");

		}

        if(elemName == "api")
        {
            pelem = elem->FirstChildElement("port");
            if (pelem)
                strcpy(cfg.api_port, (char*) pelem->GetText());
            else
                strcpy(cfg.api_port, (char*) "1132");

            pelem = elem->FirstChildElement("enabled");
            if (pelem)
			{
				strcpy(gen_buffer,(char *)pelem->GetText());
				lcase(gen_buffer);
                if (strcmp( gen_buffer, "yes") == 0)
					cfg.api_enabled=TRUE;
			}
            else
				cfg.api_enabled=FALSE;
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
    printf("validator_left: %d\n",cfg.validator_left);
    printf("validator_right: %d\n",cfg.validator_right);
    printf("validator_ucd: %d\n",cfg.validator_ucd);
    printf("ucd1: %s\n", cfg.ucd1);
    printf("ucd2: %s\n", cfg.ucd2);

    printf("utd: %d\n", cfg.utd);


	printf("outter: %s\n",cfg.outterlock);
	printf("inner: %s\n",cfg.innerlock);
	printf("shutter: %s\n",cfg.shutterlock);
	printf("sidecar: %s\n",cfg.sidecarlock);
	printf("base: %s\n",cfg.baselock);

//    printf("api: %s\n",cfg.api);
//    printf("recycler: %s\n", cfg.recycler);
    printf("printer: %s\n", cfg.printer);
    printf("tampersw: %s\n", cfg.tampersw);

	// logs
	printf("LOGS\n");
    printf("logdbf: %s\n", cfg.logdbf);
    printf("logenabled: %s\n", cfg.logenabled);
	printf("logfile: %s\n",cfg.logfile);
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
	if ( strcmp(cfg.clientportal,"enabled") ==0)
		delete cloud_server;	// kill our connection to the cloud
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
	float dollars;
} utd_inv[MAX_UTD_COLS];


void Get_UTD_Data(void)
{
    char query[]="SELECT col,tube_name,tube_value  FROM utd_denom";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer
//    printf("Data Returned: %s\n",localDBF.row[1]);

	for (int n=0; n < numrows; n++)
	{
		utd_inv[n].col= atoi(localDBF.row[0]);				// column
        utd_inv[n].tube_name= string(localDBF.row[1]);    	// tube_name
		utd_inv[n].tube_value= atoi(localDBF.row[2]);		// tube_value
//		utd_inv[n].dollars= (utd_inv[n].tube_count * utd_inv[n].tube_value) /100.0;
//		printf("col %d:: %s  Dollars %4.2f\n",utd_inv[n].col,utd_inv[n].tube_name.c_str(),  utd_inv[n].dollars);
		GetRow(&localDBF);	// get next row
	}

}




/*
	read the langs table of all supported languages and populate
	langs[] struct

*/
void GetLangs(void)
{
    char query[]="SELECT code,name,active  FROM langs WHERE active='1'";
    int result =  QueryDBF(&localDBF,query);
    int numrows = GetRow(&localDBF);    // populate gen_buffer
	lang_count=numrows;

	for (int n=0; n < numrows; n++)
	{
		langs[n].code = string(localDBF.row[0]);
		langs[n].name = string(localDBF.row[1]);
		langs[n].active = atoi(localDBF.row[2]);
		langs[n].id=atoi(localDBF.row[3]);

		printf("Lang: %s  Code:%s  Active: %d\n",langs[n].name.c_str(),langs[n].code.c_str(),langs[n].active);
		GetRow(&localDBF);	// get next row
	}

}


/*
    install the keyboard/numpad as defined in kb struct
*/

void InstallKB(void)
{
	if (kb.abc123==KEYBOARD)
	{
    	REPARENT(app_ptr->keyboard_window, kb.attached, kb.active);
	}
	else
	{
        REPARENT(app_ptr->numpad_window, kb.attached, kb.active);
	}
}


/*
	whichever is active onscreen (keyboard or keypad), reparent back to its home window from kb.target
	to make room to parent it somewhere else (a new window)

    widget names for key/num pad usage
	----------------------------------------------------------

	WIDGET					HOME WINDOW
	========================================
    keyboard_grid			keyboard_window
    numpad_grid				numpad_window

*/
void ReHomeKB(void)
{

// REPARENT(old_parent,new_parent,widget)


		if (kb.abc123== KEYBOARD)
		{
			// rehome the keyboard widget
			REPARENT(GTK_CONTAINER(kb.attached),  app_ptr->keyboard_window, app_ptr->keyboard_grid);
		}
		else
		{
			// rehome the keypad widget
			REPARENT(GTK_CONTAINER(kb.attached), app_ptr->numpad_window,app_ptr->numpad_grid);
		}

}


/*

	send tbl to the remote server
	tbl can actually be several tables separated by space

	RETURNS: TRUE on success, else FALSE
*/

bool SendTableToServer(string tbl)
{
	string tbl_name;
	string fcontents;
	bool ret;

	tbl_name= DumpSQLTable(tbl);	// uses mysqldump to dump table to tmp/tbl.sql

	if (tbl_name=="error")
	{
		return FALSE;

	}

	printf("TABLE NAME:: %s\n",tbl_name.c_str() );

	// read the file into a string
	fcontents=ReadFile(tbl_name);

//TODO must log this
	if (fcontents == "file not found")
	{
		printf("ERROR: file not found\n");
		return FALSE;
	}

	ret=SendCloud("FILE", tbl, (char *) fcontents.c_str() );
	if (!ret) return FALSE;

	return TRUE;
}



/*
    use mysqldump to dump 'tbl' to 'tmp/tbl.sql'

    returns pathname of the created table
    returns "error" on error

	NOTE: tbl can be a single name or multiple separated by spaces
*/

string DumpSQLTable(string tbl)
{
    FILE * nix;
    char run_string[100];
    char tbl_name[50];

	string fixed = tbl;

    // convert the passed string, converting spaces to underscore
    ReplaceAll( fixed, " ", "_");

    // create the system string of the command to run
    sprintf(run_string,"mysqldump -u'%s' -p'%s' %s %s > tmp/%s.sql", cfg.usertxt, cfg.pwtxt, cfg.dbftxt, tbl.c_str(), fixed.c_str() );

printf("RUNNING:: %s\n",run_string);

    sprintf(tbl_name,"tmp/%s.sql",fixed.c_str() );

    // popen() creates a pipe so we can read the output
    if ( !(nix = popen(run_string,"r")))
        return "error";


    pclose(nix);

    return string(tbl_name);

}





/*
	connect to our cloud server

*/
void ConnectCloud(void)
{
    int res;
	string msg1;

	if (!cfg.remoteserver) return;


    res=cloud_server->Client(cfg.cloud_ip, atoi(cfg.cloud_port) );
    if (res !=0 )
    {
       // res has the error code (usable with getMessage()
		sprintf(gen_buffer,"Unable to connect to remote server on %s:%s",cfg.cloud_ip,cfg.cloud_port);
		WriteSystemLog(gen_buffer);
printf("%s\n",gen_buffer);

		msg1=getMessage(res,FALSE);
		sprintf(gen_buffer,"%s",msg1.c_str() );
		WriteSystemLog(gen_buffer);
		printf("%s\n",gen_buffer);
    }
	else
	{
		sprintf(gen_buffer,"Success:: connected to the remote server on %s:%s",cfg.cloud_ip,cfg.cloud_port);
        WriteSystemLog(gen_buffer);
printf("%s\n",gen_buffer);

	sleep(2);
    char msg[200]="hello world";
    printf("Sending: %s\n",msg);
    SendCloud("MSG","",msg);
/*
	sleep(2);

	printf("Sending: %s\n",msg);
	SendCloud(msg);
*/
 	}

}


/*
	Supported packet types
	"MSG" - simply a text message
	"FILE" - a file is in the payload section


	PACKET FORMAT
	type	char [30]	- will determine destination for the payload
	fname	char [30]	- if type= 'FILE' => goes to this filename
	payload	xxxx


	type[30] fname[50] payload[xx]


	NOTE: msg must be an ASCIZ string and therefore cannot contain binary data

*/

#define CLOUD_PTYPE_LEN 30
#define CLOUD_FNAME_LEN 50

bool SendCloud(string ptype, string fname, char * msg)
{

	bool res;
	char pkttype[CLOUD_PTYPE_LEN];
	char filename[CLOUD_FNAME_LEN];

	int pktlen= CLOUD_PTYPE_LEN+CLOUD_FNAME_LEN+strlen(msg);
	char packet[pktlen];

	bzero(pkttype,CLOUD_PTYPE_LEN);
	bzero(filename,CLOUD_FNAME_LEN);
	strncpy(pkttype,ptype.c_str(),CLOUD_PTYPE_LEN);
	strncpy(filename,fname.c_str(),CLOUD_FNAME_LEN);

	// now assemble the packet
	memcpy(packet,pkttype,CLOUD_PTYPE_LEN);
	memcpy(&packet[CLOUD_PTYPE_LEN],&filename[0],CLOUD_FNAME_LEN);
	memcpy(&packet[CLOUD_PTYPE_LEN+CLOUD_FNAME_LEN],msg,strlen(msg));


    res=cloud_server->SendMessageBinary(packet,pktlen);	// returns bool, TRUE= success, else FALSE
    if (!res)
    {
		printf("ERROR sending data\n");
		return FALSE;
    }
	else
	{
		printf("data sent successfully\n");
	}

int n;
for (n=0; n<10; n++)
{

    int bytecount;
    char *ptr;
    ptr=cloud_server->ReceiveMessage(&bytecount);
    if (ptr == NULL)
    {
        // no data
    }
    else
    {
        //ptr points to our char[] data
        // bytecount has number of bytes in buffer
		printf("RCVD: %s\n",ptr);
    }

	sleep(1);
}

	return TRUE;
}



int GetAPIport(void)
{
    return atoi(cfg.api_port);
}


// take input from the API port
void API_Handler(void)
{
	static int callnumber=0;
	static bool inprocess=FALSE;

	// first, check our conditionals to see if we have anything to do
	if (inprocess) return;				// we are not re-entrant
	if (!cfg.api_enabled) return;		// skip if not enabled
	if (++callnumber % 5 != 0) return;	// if not the 5th call (eg 500ms, then return

    inprocess=TRUE;

//printf("API_HANDLER here\n");
	ListenAPI();	// in api.cpp
					// returns char* to received string or NULL

	inprocess=FALSE;	// returning control to your normal broadcasting station

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

