// Commands
#define CMD_STATUS  	0x38
#define CMD_SEARCH  	0x3A
#define CMD_ACK     	0x43
#define CMD_VERSION     0x4B
#define CMD_DISPENSE    0x86
#define CMD_UNLOAD      0x87
#define CMD_LOAD        0x88
#define CMD_LOADEND     0x8B
#define CMD_RESET       0x9A	// reboot device

// added by GRC
#define CMD_VEND_DELAY		0x72	// set vend delay in minutes (must be ACKd)
#define CMD_GET_VEND_DELAY 	0x73	//  request the vend delay value from device
#define RPL_VEND_DELAY 		0x74	// response to req for vend delay value
#define CMD_UNLOAD_DELAY	0x8C	// request the UNLOAD delay
#define CMD_SET_UNLOAD_DELAY 0x8D	// set unload delay in minutes
#define RPL_UNLOAD_DELAY	0x8E	// response to req for UNLOAD DELAY
#define CMD_SET_TIME_LOCK	0x4E	// set time lock for device
#define CMD_GET_TIMELOCK	0x50	// get timelock value from device
#define RPL_TIMELOCK		0x51	// resp to req for timelock value
#define CMD_SET_COLUMN		0x78	// set column counts BCD encoded
#define CMD_GET_COLUMN		0x79	// req column values (inventory)
#define RPL_COLUMN			0x7A	// resp to req for column values
#define CMD_SET_VAR_VEND_DELAY 0x75	// set a variable vend delay
#define CMD_REQ_VAR_VEND_DELAY 0x76	// req variable vend delay value
#define RPL_VAR_VEND_DELAY	0x77	// resp to req for var vend delay value
 
//#define RPL_SEARCH	0x3F

#define CMD_SET_ADDRESS		0x46	// set devaddr
#define RPL_NACK			0x40	// NACK a msg


    // Replies
#define RPL_SEARCH      0x3E
#define RPL_ACK         0x41
#define RPL_STATUS      0x71
#define RPL_VERSION     0x4C

    // Statuses
#define STS_IDLE        0
#define STS_DISPENSING  0x10
#define STS_JAM         0x20
#define STS_UNLOADING   0x30
#define STS_DISPDELAY   0x44
#define STS_DISPENSED   0x50  // Must be ACKed
#define STS_UNLOADED    0x60  // Must be ACKed
#define STS_LOADING     0x80
#define STS_ALIGNING    0xA0
#define STS_UNLOAD_COMPLETE	0xB0  // Must be ACKed
#define STS_DISPUNVERIFIED  0xD0  // Must be ACKed (dispenser vended, but did not detect tube dropping)

char PKT_STH = 0x01;
char PKT_STX= 0x02;
char PKT_ETX = 0x03;



// Prepared messages (The parameters were sniffed -- there's no documentation on what they actually mean)
//    char MSG_SEARCH[]          = { (char) CMD_SEARCH, 0, 0x30, 0, 0, 0, 0, 0};
//    char MSG_SEARCHING[]     =  { (char) CMD_SEARCH, 0x0F, (char) 0xF0, 00, 00, 00, 00, 00 };


    string MSG_SEARCH          = { (char) CMD_SEARCH, 0, (char) 0x30, 0, 0, 0, 0, 0};
    string MSG_SEARCHEND     =  { (char) CMD_SEARCH, 0x0F, (char) 0xF0, 00, 00, 00, 00, 00 };

//01 10 AA AA AA AA 02 3A 00 30 00 00 00 00 00 03 1A

typedef struct
{
	string dispensein;     	//space separated list of tubes to dispense
	string dispenseamt;    	//penny amount to dispense to user (ie 12300)
	string values;         	//list of column penny value denominations space separated (100 500 ...)
	string counts;         	//list of column tube counts space separated (10 5 300 0 0 0 0 0)
	string unload;         	//column number to unload

	string statusout;     	//current status
	string serialout;     	//unit's serial number
	string fwverout;      	//unit's firmware version
    string dispenseout;   	//space separate list of dispense counts (1 2 3 4 5 6 7 8)
    string dispenseXout;  	//items dispensed from column x (1 to 8) on last dispense
    string dispenseamtout; //amount dispensed on last dispense req (penny amt)
    string failreasonout; 	//reason for fail on DISPENSEFAIL, UNLOADFAIL and ERROR
    string inventoryout;  	//space separated list of intentory counts (1 2 3 4 5 6 7 8)
    string columnout;     	//column number for tube LOAD event
    string unloadcountout;	//number of tubes detected during unload
    string errstrout;     	//reports same as failreason.out on ERROR

} d8_struc;


