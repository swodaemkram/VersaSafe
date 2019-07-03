/*
	this structure is used for reading the xml/config.xml file INTO
	and access from various modules
	to use, simply


	#include "config.inc"

    the instantiation of cfg is in vsafe.cpp

*/



#include "hdr/fire.h"	// for TRUE,FALSE

typedef struct
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
    bool utd;
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
    char logenabled[10];        // enabled/disabled
    char logfile[40];
    char logremotedbf[10];

    // remote
    char clientportal[10];  // enabled/disabled
    char maintportal[10];
    char dbfportal[10];

    // remoteserver
	bool remoteserver=FALSE;
    char cloud_ip[20];
    char cloud_port[10];

    // timeouts
    char validator_timeout[10];

    // api
    char api_port[10];
    bool api_enabled=FALSE;
} config;


extern config cfg;

