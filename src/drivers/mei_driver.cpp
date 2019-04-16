/*
 * MEIValidator.cpp
 *
 *  Created on: Apr 9, 2019
 *Author: Mark Meadows
 *
 *A driver for the
 *  MEI Validator
 *Comm setup is 9600 E 7 1
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
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

#include "mei_driver.h"
#include "../hdr/global.h"
#include "../trim.inc"   //Looks as if I am unable to use Gary's trim function for some reason with my IDE no big deal
                         //more then one way to skin a cat
#include "../logs.inc"

using namespace std;
using namespace LibSerial;

/*
===============================================================================================================================
This was a function I used to make a map of were everything goes
===============================================================================================================================
*/
class mei {

public:
	void markiscool(void){
		printf("\n!MARKISCOOL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}

/*
===============================================================================================================================
Start of MEI crc Function
===============================================================================================================================
*/

private:

unsigned do_crc(char buff[], int buffer_len){
			 //printf("\n%02x%02x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]); //DEBUG CODE
			 //printf("%d\n",buffer_len); //DEBUG CODE
			 int i=1;
			 unsigned int sum = 0;
			 buffer_len = buffer_len - 2;

			        while ( i < buffer_len)
			        {
			           sum ^= buff[i];
			           //printf("%02x,%02x\n",sum,buff[i]); //DEBUG CODE
			           i++;
			        }
			        //printf("%d\n",buffer_len); //DEBUG CODE
		            //printf("the check sum = %02x\n",sum); //DEBUG CODE

	return(sum);
		}
/*
===============================================================================================================================
End of MEI crc Function
===============================================================================================================================
*/














};
/*
===============================================================================================================================
 Connection to MEI Validator Completed
===============================================================================================================================
*/




















