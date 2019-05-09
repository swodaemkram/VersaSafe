/*
    Program: VSAFE
    Module: api.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 4-8-2019
    Updated:
    Language: C,C++ (this module is C++ compilable and linkable with C libs)


*/


#include "hdr/config.h"
#include "usb_gateway.inc"
#include "hdr/usb_gateway.h"


SOCKET * api = new SOCKET(TCP_CONNECTION); // instantiate our cloud socket

int api_port;


void CreateAPI(void)
{
	int res;

	res=api.Server(api_port);

	if (res !=0 0)
	{
		// res has the error code
	}
}

