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
#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
/* MEI Constants */

	// Byte 0 Status
#define MEI_ACCEPTING = 0x02;
#define MEI_ESCROW    = 0x04;
#define MEI_STACKING  = 0x08;
#define MEI_STACKED   = 0x10;
#define MEI_RETURNING = 0x20;
#define MEI_RETURNED  = 0x40;
	// Byte 1 Status
#define MEI_CHEATED   = 0x01;
#define MEI_REJECTED  = 0x02;
#define MEI_JAM	   = 0x04;
#define MEI_FULL	   = 0x08;
#define MEI_CASSETTE  = 0x10;
	// Byte 2 Status
#define MEI_POWERUP   = 0x01;
#define MEI_FAILURE   = 0x04;
	// Byte 3 Status
#define MEI_DOWNLOADING = 0x02;
	// Commands
#define	MEI_POLL      = 0x10;
#define MEI_STACK     = 0x30;
#define MEI_RETURN    = 0x50;
	// Command Types
#define MEI_CMD       = 0x10;
//#define 	MEI_REPLY     = 0x20;
#define MEI_DOWNLOAD  = 0x50;
#define MEI_AUX       = 0x60;
	// Auxiliary command sub-types
#define MEI_GETMODEL  = 0x04;
#define MEI_GETSERIAL = 0x05;
#define MEI_GETBOOTVER	= 0x06;
#define MEI_GETAPPVER = 0x07;
#define MEI_GETVARNAME	= 0x08;
#define	MEI_GETVERSION	= 0x09;
#define MEI_GETQP		= 0x0B;
#define MEI_GETPERF   =  0x0C;
#define MEI_GETBNF		= 0x10;
#define	MEI_RESET		= 0x7F;
#define	MEI_EXT       =  0x70;
	// Extension command sub-types
#define	MEI_GETBILLS  =  0x02;
#define	MEI_RETRIEVE  =  0x0B;
#define	MEI_BOOKMARK  =  0x0D;

//void ack_message_send(char *comm_port,char *pkt);
//char * ack_packet_build(unsigned int pkt_command);
//char *build_packet_cmd(unsigned int pkt_command);
//char *build_packet_ext_cmd(unsigned int pkt_command);
//char *build_packet_reset(unsigned int pkt_command);
//char * build_packet(unsigned int pkt_command);
//void print_help();
//void send_pkt (char *comm_port,char *pkt, unsigned int pkt_command);
//void setup_serial_port(char *comm_port);


int mei_do_crc(char buff[], int buffer_len);

typedef struct
{


} mei_struc;


#endif /* DRIVERS_MEI_DRIVER_H_ */
