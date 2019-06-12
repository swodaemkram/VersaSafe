
#include "hdr/fire.h"

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
    char logenabled[10];        // enabled/disabled
    char logfile[40];
    char logremotedbf[10];

    // remote
    char clientportal[10];  // enabled/disabled
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
} config;


extern config cfg;
