/*
    Module: usb.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 12-7-2017
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2017 FireKing Security Group

    Version: 1.0


	this module is handles basic USB interactions

	References
	http://www.dreamincode.net/forums/topic/148707-introduction-to-using-libusb-10/
	http://libusb.sourceforge.net/api-1.0/index.html


	USB COMMUNICATIONS
	Communication between devices and client software is conceptualized as using pipes. Each pipe is a communication channel between software on 		the host and an endpoint on a device. Each endpoint represents a part of a device that fulfills one specific purpose for that device, such 		as to receive commands or transmit data. A full speed device can have up to 16 endpoints, though low speed devices can have only three.

	All USB devices support endpoint 0 when powered up. This endpoint is the target of the default pipe. After the attachment of a device has 		been detected, the USBD software uses endpoint 0 to initialise the device, perform generic (i.e. non device-specific) configuration, and 		obtain information about the other endpoints provided by the device. Endpoints are characterised by their endpoint number (set at design 		time) and bus bandwidth, access frequency, latency and error handling behaviour requirements.

	Once the endpoints of a device have been identified and configured, pipes come into existence allowing the client software to communicate 		with the device. A pipe has associated with it characteristics such as a claim on bus access and bandwidth, the type of transfer, the 		direction of transfer and the maximum data payload size.

	USB defines four types of transfer: 
	1. control transfers which are typically used for command or status operations, 
	2. interrupt transfers which are initiated by a device to request some action from the host, 
	3. isochronous transfers which are used to carry data the delivery of which is time critical (such as for video and speech),  
	4. bulk transfers which can use all available bandwidth but are not time critical. 

	All transfers take the form of packets, which contain control information, data and error checking fields.

	There are also two types of pipe: message pipes and stream pipes. Control transfers are made using message pipes. In a message pipe, the 		data portion of each packet has some meaning to the USB system software.

	Stream pipes are used for interrupt, isochronous and bulk transfers. In a stream pipe, the data portion of the packet has no defined meaning 		to the USB: the data is merely conveyed between client software and device.



	Devices and interfaces
	Each usb device is manipulated with a libusb_device and libusb_device_handle objects in libusb. The libusb API ties an open device to a 	specific interface. This means that if you want to claim multiple interfaces on a device, you should open the device multiple times to 		receive one libusb_dev_handle for each interface you want to communicate with. Don't forget to call usb_claim_interface.
	What does this mean? Means you have to claim the interface before you do any operation on the device, and also, you have to release the 	claimed interface after you are finished with the device.

	OK, now what? yup, we have to find these, so called devices/interfaces so that we can claim them and assign to a handle and so on... here is 		the magic... :D

	Each device has it's own configuration values, like vendor id, product id, etc. We use these settings to discover the desired device to	
	work with. 

	To discover the devices on the USB network...

	1.initialize the library by calling the function libusb_init and creating a session
	2.Call the function libusb_get_device_list to get a list of connected devices. This creates an array of libusb_device containing all usb 		devices connected to the system.
	3.Loop through all these devices and check their options
	4.Discover the one and open the device either by libusb_open or libusb_open_device_with_vid_pid(when you know vendor and product id of the 		device) to open the device
	5.Clear the list you got from libusb_get_device_list by using libusb_free_device_list
	6.Claim the interface with libusb_claim_interface (requires you to know the interface numbers of device)
	7.Do desired I/O
	8.Release the device by using libusb_release_interface
	9.Close the device you openedbefore, by using libusb_close
	10.Close the session by using libusb_exit


Device classes
Id 	Name
00	(Defined at Interface level)
01	Audio
02	Communications
03	Human Interface Device
05	Physical Interface Device
06	Imaging
07	Printer
08	Mass Storage
09	Hub
0a	CDC Data
0b	Chip/SmartCard
0d	Content Security
0e	Video
58	Xbox
dc	Diagnostic
e0	Wireless
ef	Miscellaneous Device
fe	Application Specific Interface
ff	Vendor Specific Class




SV2 devices

vault lock
Bus 002 Device 007: ID 04d8:000a Microchip Technology, Inc. CDC RS-232 Emulation Demo
VID: 4d8  PID: a Class: 2 (Comm)



*/


#include <iostream>
//#include <libusb.h>
#include <stdio.h>
#include </usr/include/libusb-1.0/libusb.h>
#include "hdr/fire.h"

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <stdlib.h>
#include <string.h>
#include <vector>

//#include "trim.inc"


using namespace std;

// in trim.cpp
extern vector <string> split( const string &src, const string &delim);


//public
int initUSB(bool silent);
void getUSBversion(void);
void ShowVidPids(void);
void FindLocks(void);
//void FindLocks1(void);


//private
void printdev(libusb_device *dev); //prototype of the function
string GetClass(int myclass);
void ParseLockPorts(char * line);


/*
struct libusb_version {
    // Library major version. 
    const uint16_t major;

    // Library minor version. 
    const uint16_t minor;

    // Library micro version. 
    const uint16_t micro;

    // Library nano version. 
    const uint16_t nano;

    // Library release candidate suffix string, e.g. "-rc4". 
    const char *rc;

    // For ABI compatibility only. 
    const char* describe;
};
*/


//const struct 
const libusb_version * LUSB_VERSION;


struct
{
	int index;
	int vid;
	int pid;
	int dclass;
} vidpids[40];
int vpindex;

struct
{
	int id;
	string desc;
} dclasses[20]=
{0,"Defined at Interface",
1,"Audio",
2,"Comm",
3,"HID",
5,"Physical Interface Dev",
6,"Imaging",
7,"Printer",
8,"Mass Storage",
9,"Hub",
10,"CDC Data",
11,"Chip/Smart Card",
13,"Content Security",
14,"Video",
0x58,"Xbox",
0xe0,"Wireless",
0xef,"Misc",
0xfe,"App Specific Interface",
0xff,"Vendor Specific Interface"}
;

extern char gen_buffer[255];

/*
	run the findlocks script and find all FKI devices and their port

NR: /dev/ttyACM0 - FKI_Security_Group_LLC._Apunix_LPM-01_10


*/



void FindLocks()
{
    #define BUFF_SIZE 512
    char *buff= new (nothrow) char[BUFF_SIZE];
	char network_results[256];
	char script[40]="./findlocks";
	FILE *in;

	if ( !(in =  popen(script,"r")))
	{
		delete[] buff;
	}

	bzero(buff,BUFF_SIZE);
	while (fgets(buff,BUFF_SIZE,in) != NULL)
	{
		if (strlen(buff) <4) continue;
		bzero(network_results,sizeof(network_results));
		strcpy(network_results,buff);
        printf("NR: %s\n",network_results);
		ParseLockPorts(network_results);
	}

	pclose(in);
	delete[] buff;

}


typedef struct
{
	string port;
	string vendor;
	bool enabled;
} ulock;


ulock usblocks[MAX_LOCKS];

/*
NR: /dev/ttyACM0 - FKI_Security_Group_LLC._Apunix_LPM-01_10

	parse the above passed in string into usblocks[]

*/

vector <string> lock;
void ParseLockPorts(char * line)
{
	for (int n=0; n < MAX_LOCKS; n++)
		usblocks[n].enabled=FALSE;

	static int index=0;

	lock=split(line," ");
	// lock[0] = "/dev/ttyACM0"
	// lock[1] = "-"
	// lock[2] = "FKI_Security_Group_LLC._Apunix_LPM-01_10"

	usblocks[index].port=lock[0];
	usblocks[index].vendor=lock[2];
	usblocks[index].enabled=TRUE;
	index++;
	printf("PORT: %s\n",lock[0].c_str() );
	printf("VENDOR: %s\n",lock[2].c_str() );
}




//https://unix.stackexchange.com/questions/61484/find-the-information-of-usb-devices-in-c

/*
idVendor           0x04d8 Microchip Technology, Inc.
  idProduct          0x000a CDC RS-232 Emulation Demo
    iManufacturer           1 FKI Security Group LLC. Apunix LPM-01
*/

/*
//#include <stdio.h>
//#include <hdr/usb.h>
void FindLocks1()
{
    struct usb_bus *bus;
    struct usb_device *dev;
    usb_init();
    usb_find_busses();
    usb_find_devices();
    for (bus = usb_busses; bus; bus = bus->next)
        for (dev = bus->devices; dev; dev = dev->next){
            printf("Trying device %s/%s\n", bus->dirname, dev->filename);
            printf("\tID_VENDOR = 0x%04x\n", dev->descriptor.idVendor);
            printf("\tID_PRODUCT = 0x%04x\n", dev->descriptor.idProduct);
        }
}
*/

/*
	retrieve the libusb driver version information and store to gen_buffer
*/

void getUSBversion(void)
{
	LUSB_VERSION =libusb_get_version();
	sprintf(gen_buffer,"%d.%d.%d.%d",LUSB_VERSION->major,LUSB_VERSION->minor,LUSB_VERSION->micro,LUSB_VERSION->nano);
//	printf("%s",gen_buffer);
	
}





int initUSB(bool silent)
{
	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	ssize_t cnt; //holding number of devices in list
	r = libusb_init(&ctx); //initialize a library session

	if(r < 0) 
	{
		if (!silent)
		        cout<<"Init Error "<<r<<endl; //there was an error
                return 1;
	}

	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices

	if(cnt < 0) 
	{
		if (!silent)
			cout<<"Get Device Error"<<endl; //there was an error
	}

	if (!silent)
		cout<<cnt<<" Devices in list."<<endl; //print total number of usb devices

	ssize_t i; //for iterating through the list

	vpindex=0;

printf("=================================\n");
	for(i = 0; i < cnt; i++) 
	{
		if (!silent)
		printdev(devs[i]); //print specs of this device
	}

printf("=========================================\n");

	if (!silent)
		ShowVidPids();

	libusb_free_device_list(devs, 1); //free the list, unref the devices in it
	libusb_exit(ctx); //close the session

	return 0;


}
	 

/*
	Show all discovered VID/PID combos on the USB network
*/

void ShowVidPids(void)
{
	int i;
	printf ("USB Devices: %d\n",vpindex);

	for (i=0; i < vpindex; i++)
	{
		string myclass = GetClass(vidpids[i].dclass);

		printf("VID: %04x  PID: %04x Class: %d (%s)\n",vidpids[i].vid,vidpids[i].pid, vidpids[i].dclass,myclass.c_str());
	}
	printf("\n");

}


/*
	convert the numeric USB class into human readable form
*/
string GetClass(int myclass)
{
	for (int i=0; i < 20; i++)
	{
		if (dclasses[i].id == myclass)
			return dclasses[i].desc;
	}
	return "unkown";
}



void printdev(libusb_device *dev) 
{

	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);


	if (r < 0) 
	{
		cout<<"failed to get device descriptor"<<endl;
		return;
	}

	vidpids[vpindex].vid = desc.idVendor;
	vidpids[vpindex].pid = desc.idProduct;
	vidpids[vpindex].dclass = desc.bDeviceClass;
	vpindex++;
	cout<<"Number of possible configurations: "<<(int)desc.bNumConfigurations<<"  ";
	cout<<"Device Class: "<<(int)desc.bDeviceClass<<"  ";
	cout<<"VendorID: "<< std::hex <<desc.idVendor<<"  ";
	cout<<"ProductID: "<< std::hex <<desc.idProduct<<endl;
	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	cout<<"Interfaces: "<<(int)config->bNumInterfaces<<" ||| ";
	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;

	for(int i=0; i<(int)config->bNumInterfaces; i++) 
	{
		inter = &config->interface[i];
		cout<<"Number of alternate settings: "<<inter->num_altsetting<<" | ";

		for(int j=0; j<inter->num_altsetting; j++) 
		{
	            interdesc = &inter->altsetting[j];
	            cout<<"\nInterface Number: "<<(int)interdesc->bInterfaceNumber<<" | ";
	            cout<<"Number of endpoints: "<<(int)interdesc->bNumEndpoints<<" | ";

	        	for(int k=0; k<(int)interdesc->bNumEndpoints; k++) 
			{
		                epdesc = &interdesc->endpoint[k];
		                cout<<"Descriptor Type: "<<(int)epdesc->bDescriptorType<<" | ";
		                cout<<"EP Address: "<<(int)epdesc->bEndpointAddress<<" | ";
			}
			cout << "\n" << " ";

	        }

	}

	    cout<<endl<<endl<<endl;
	    libusb_free_config_descriptor(config);

}


	 
//EOM

