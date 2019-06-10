/*
 * MEIValidator.h
 *
 *  Created on: Apr 9, 2019
 *      Author: mark
 */

#ifndef DRIVERS_MEI_DRIVER_H_
#define DRIVERS_MEI_DRIVER_H_

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <SerialStream.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <bits/stdc++.h>    // for strcpy
#include <array>            // for array copy
#include <iostream>
#include <vector>
#include <SerialPort.h>
#include "../hdr/global.h"
#include "../trim.inc"
#include "../logs.inc"
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <termios.h>
#include <fcntl.h>


using namespace std;
using namespace LibSerial;

typedef struct
{


} mei_struc;


#endif /* DRIVERS_MEI_DRIVER_H_ */
