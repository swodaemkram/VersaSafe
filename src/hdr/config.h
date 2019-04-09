/*
	Module: config.h
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 2-1-2012
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2012 FireKing Security
    Version: 1.0

	This file holds the object declarations for the various pieces of hardware

*/



#include "socket_class.h"



// this data structure is used by FindHelp() in help.cpp
// it's populated as the function iterates the help file
// and locates all of the sections and pages

typedef struct
{
	string section;
	string page;
	string title;
	string header;
	string body;
	string footer;
} help_data;

// the following structure is used for building the help TOC
// GetHelpContents() in help.cpp fills this structure in
typedef struct
{
	string section;			// holds the "section" name
	string name;			// holds the "name" attribute
	string page_title[20];
	int page_count;
} contents_data;




