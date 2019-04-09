/*
 * MEIValidator.h
 *
 *  Created on: Apr 9, 2019
 *      Author: mark
 */

#ifndef DRIVERS_MEIVALIDATOR_H_
#define DRIVERS_MEIVALIDATOR_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>



/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
/* MEI Constants */

	// Byte 0 Status
int MEI_ACCEPTING = 0x02;
int MEI_ESCROW    = 0x04;
int MEI_STACKING  = 0x08;
int MEI_STACKED   = 0x10;
int MEI_RETURNING = 0x20;
int MEI_RETURNED  = 0x40;
	// Byte 1 Status
int MEI_CHEATED   = 0x01;
int MEI_REJECTED  = 0x02;
int MEI_JAM	   = 0x04;
int MEI_FULL	   = 0x08;
int MEI_CASSETTE  = 0x10;
	// Byte 2 Status
int MEI_POWERUP   = 0x01;
int MEI_FAILURE   = 0x04;
	// Byte 3 Status
int MEI_DOWNLOADING = 0x02;
	// Commands
int	MEI_POLL      = 0x10;
int	MEI_STACK     = 0x30;
int	MEI_RETURN    = 0x50;
	// Command Types
int	MEI_CMD       = 0x10;
//	MEI_REPLY     = 0x20;
int	MEI_DOWNLOAD  = 0x50;
int	unsigned MEI_AUX       = 0x60;
	// Auxiliary command sub-types
unsigned int	MEI_GETMODEL  = 0x04;
unsigned int    MEI_GETSERIAL = 0x05;
unsigned int	MEI_GETBOOTVER	= 0x06;
unsigned int	MEI_GETAPPVER = 0x07;
unsigned int	MEI_GETVARNAME	= 0x08;
unsigned int	MEI_GETVERSION	= 0x09;
unsigned int	MEI_GETQP		= 0x0B;
unsigned int	MEI_GETPERF   =  0x0C;
unsigned int	MEI_GETBNF		= 0x10;
unsigned int	MEI_RESET		= 0x7F;
unsigned int	MEI_EXT       =  0x70;
	// Extension command sub-types
int	MEI_GETBILLS  =  0x02;
int	MEI_RETRIEVE  =  0x0B;
int	MEI_BOOKMARK  =  0x0D;

void ack_message_send(char *comm_port,char *pkt);
char * ack_packet_build(unsigned int pkt_command);
char *build_packet_cmd(unsigned int pkt_command);
char *build_packet_ext_cmd(unsigned int pkt_command);
char *build_packet_reset(unsigned int pkt_command);
char * build_packet(unsigned int pkt_command);
void print_help();
void send_pkt (char *comm_port,char *pkt, unsigned int pkt_command);
void setup_serial_port(char *comm_port);
int do_crc(char buff[], int buffer_len);



#endif /* DRIVERS_MEIVALIDATOR_H_ */
