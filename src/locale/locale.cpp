/*
	Module: locale.cpp
	Author: Gary Conway <gconway@flowcorp.com>
	Created: 6-18-2012
	Updated: 9-5-2013
	Version: 1.0
	Notice: Copyright 2012 Flow International, All Rights Reserved


	This module reads the ISO 3166 country code information
	and populates the countries[] array of structures

	additionally, the module will read a language file and load it into the msgs[] structure

	NOTES on internationalization of GTK
	http://developer.gnome.org/gtk/stable/gtk-question-index.html

*/



using namespace std;

#include "../hdr/fire.h"
#include "../locale/locale.h"

// C++
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

// C
#include <stdint.h>		// has uint32_t etc.

// PUBLIC FUNCTIONS
int ReadCountryCodes(void);
void PrintCountryInfo(void);
int SetLang(const char *filename);
string getMessage(int msgID, int add_msgnum);
void PrintMessages(void);




// trim module functions
extern char *trim(char *str);
extern char *ltrim(char *str);
extern char *rtrim(char *str);
extern void chomp(char *str);
extern uint32_t htoi(char *ptr);


// common.cpp
extern int CountFileLines(char *fname, int *index, bool include_comments);
//extern char gen_buffer[];


// this is a stub, the actual array must be created dynamically from the /lang/ folder
const char * languages[] = {"english","spanish","german","chinese","japanese"};
int curLanguage=0;	// index into the above array

//#define MAX_COUNTRIES 250
//country countries[];
country * countries;
int country_count;

/*
sample file format
AF AFG 004 Afghanistan
AL ALB 008 Albania, People's Socialist Republic of
DZ DZA 012 Algeria, People's Democratic Republic of
AS ASM 016 American Samoa
AD AND 020 Andorra, Principality of

*/
char iso_file[]="../locale/iso_country_codes_good_fmt.txt";



// the following structure and declaration are for
// reading the maestro language files

#define MSG_LINE_LEN 250

struct lang_file
{
	int msgID;
	char cmd_desc[MSG_LINE_LEN];
	char cmd_string[MSG_LINE_LEN];
};

lang_file * msgs;
int msg_count;





/*
	Read the ISO country file. Allocate and fill in the country[] array of structures

	RETURNS:
		0 success
		1 file not found
		2 file read error
*/



int ReadCountryCodes(void)
{

	#define MAX_CO_LINE 200
    FILE *myfile=NULL;
	char buffer[MAX_CO_LINE];
	unsigned int bytes_read;
	unsigned int filesize;

	CountFileLines( iso_file, &country_count,FALSE);
	countries = new country[country_count];

    myfile=fopen(iso_file,"r");
    if (!myfile)
    {
		printf("Unable to open ISO country file %s\n",iso_file);
		return 1;
    }

	int n=0;
do
{
	bzero(buffer,MAX_CO_LINE);
	if (fgets(buffer,MAX_CO_LINE, myfile) == NULL)
	{
		// file read error
	}

	if (feof(myfile))
	{
	}

    if (ferror(myfile))
	{
		printf("error reading country file %s\n",iso_file);
		fclose(myfile);
		return 2;
	}

	chomp(buffer);
	trim(buffer);

	if (buffer[0] == '#')	continue;		// skip comments
	if (strlen(buffer) ==0)	continue;		// and blank lines

	strncpy(countries[n].two_digit,buffer,2);
	strncpy(countries[n].three_digit, &buffer[3],3);
	countries[n].country_code= atoi(&buffer[7]);
	strcpy(countries[n].country,&buffer[11]);
	countries[n].active=TRUE;
	n++;


} while (! feof(myfile));



	fclose(myfile);
	return 0;


}



/*
	print the countries[] array of structures
	this fn for debugging only
*/

void PrintCountryInfo(void)
{


	for (int n=0; n < country_count; n++)
	{
		if (countries[n].active == TRUE)
			printf("%s %s %d %s\r\n",countries[n].two_digit,countries[n].three_digit,countries[n].country_code,countries[n].country);
	}

}


/*
	printf all messages in the msg[] struc
*/

void PrintMessages(void)
{

	// iterate all messages
	for (int n=0; n < msg_count; n++)
	{
		printf("%d,%s,%s\r\n",
			msgs[n].msgID,
			msgs[n].cmd_desc,
			msgs[n].cmd_string
		);
	}
}





/*
	retrieve the message associated with the passed msgID and return the string
	These strings are read by SetLang() in the constructor

	add_msgnum = TRUE (default), add the message number into the message output
				FALSE, just return the message as read from the database

	if we find <CR> in the string, replace it with a hard carriage return


*/

string getMessage(int msgID, int add_msgnum=TRUE)
{
	string my_msg;
	char tmp[260];
	bzero(tmp,260);

	for (int n=0; n<msg_count; n++)
	{
		if (msgID == msgs[n].msgID)
		{
			if (add_msgnum)
				sprintf(tmp,"%d::%s",msgID,msgs[n].cmd_string);
			else
				sprintf(tmp,"%s",msgs[n].cmd_string);

			my_msg = string(tmp);

			// replace any <CR> with a hard carriage return
			size_t start_pos = my_msg.find("<CR>");
			if (start_pos != std::string::npos)
			{
				my_msg.replace(start_pos, my_msg.length(), "\0x0A");
			}

			return my_msg;
		}
	}

	// error, message not found, now locate the "message not found" message
	// we do not translate this mesage because if the other messages aren't found, then
	// it's likely that this one wouldn't be found either, so we hard-code it in English
	sprintf(tmp,"getMessage(%d)::msg was not found in the language file",msgID);
	my_msg = string(tmp);

//printf("msg_count:%d\n",msg_count);
//PrintMessages();

	return my_msg;

}




/*
	Read a single language file into an array
	INPUTS: filename, ex: "english", note that we must add the extension ".lang"

	RETURNS: OK or ERROR
*/

int SetLang(const char *filename)
{
	#define MAX_LINE_LEN 255
	FILE *langfile=NULL;
	char linebuf[MAX_LINE_LEN];
	int cmdID;
	int lines=0;
	char fname[255]="locale/";

	strcat(fname,filename);
	strcat(fname,".lang");

	CountFileLines(fname, &lines,FALSE);


// now allocate an array of structures for the entries (comments and blank lines not included)
//	lang_file * msgs;
	msgs = new (nothrow) lang_file[lines];
	// the nothrow option above tells new to return a NULL ptr on failure instead of throwing an exception

	if (msgs == NULL)
	{
		return ERROR;
	}


// now read the file again and process the entries
	langfile=fopen(fname,"r");
	if (!langfile)
	{
		printf("Unable to read language file: %s\n",filename);
		return ERROR;
	}


	msg_count=0;

do
{

	if (fgets(linebuf, MAX_LINE_LEN, langfile) == NULL) break; // read a line
	if (feof(langfile)) break;
	if (ferror(langfile))
	{
		fclose(langfile);
		return ERROR;
	}

	chomp(linebuf); // strip CRLF and make ASCIZ
	trim(linebuf);


	if (linebuf[0] == '#') continue; 		// skip comments
	if (strlen(linebuf) ==0) continue;		// skip if blank line

	// first, count the number of commas in the string, there should be ONLY two
	string msg = string(linebuf);
	size_t comma_count = std::count(msg.begin(), msg.end(), ',');


	// SAMPLE LINE: "100,.AN[3],Phase A Current"
	// we have a valid line, process it
	cmdID = atoi(strtok(linebuf,","));
	msgs[msg_count].msgID=cmdID;
	strncpy(msgs[msg_count].cmd_desc, strtok(NULL,","),MSG_LINE_LEN-1);
	strncpy(msgs[msg_count].cmd_string,strtok(NULL,","), MSG_LINE_LEN-1);

	if (comma_count > 2)
	{
		printf("Warning: Error in language file: msgID:%d\n",cmdID);
	}


	msg_count++;


} while (! feof(langfile));

	fclose(langfile);
	printf("Language file processed successfully\r\n");

//	PrintCountryInfo();
//	PrintMessages();


	return OK;


}






















