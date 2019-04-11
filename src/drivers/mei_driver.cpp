/*
 * MEIValidator.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mark
 */

#include "mei_driver.h"
#include "../hdr/global.h"
#include "../trim.inc"
#include "../logs.inc"



using namespace std;
using namespace LibSerial;



class MEIValidator {

public:

	int mei_do_crc(char buff[], int buffer_len){

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


};//end of crc class





