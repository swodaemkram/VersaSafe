
#include <time.h>
#include <unistd.h> // for bzero
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SerialStream.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <bits/stdc++.h>    // for strcpy
#include <array>            // for array copy



#ifndef APULOCK_CLASS
#define APULOCK_CLASS


class apuLock
{

	public:

	apuLock(string pname);
	void Setup(void);
	string getLockDoorSensors(void);
	string getFirmware(void);
	string getVolts(void);
	string getStatus(void);
	string getAllSensors(void);
	int setSerial(string serial);
	string getSerial(void);
	int Lockit(void);
	int Unlockit(void);
	int downloadFirmware(string filename);

}


#endif

