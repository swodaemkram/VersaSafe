/*

Author: Gary Conway

	this module contains all my trim functions
	these are in-place trims
	all strings are assumed to be ASCIZ
	these functions are greedy, in that, they consider anything
	smaller than space (0x20) to be whitespace

whitespace can be any of the following
0x09 - horizontal tab
0x0A - linefeed
0x0B - vertical tab
0x0C - form feed
0x0D - carriage return
0x20 - space

*/

using namespace std;

#include <string.h>
//#include <sys/types.h>
#include <stdint.h>
#include "ctype.h"
#include <string>
#include <vector>
#include <stdio.h>

char *trim(char *str);			// trim both sides of string
char *ltrim(char *str);
char *rtrim(char *str);
void chomp(char *str);			// remove end of line characters etc.
uint32_t htoi(char *str);
void ucase(char *ptr);			// convert string to ucase
void lcase(char *ptr);
string lcase(string str);
string ucase(string str);
int count_occurrences(string mystring, char mychar);
vector<string> split( const string &src, const string &delim);	// split a string by delimiters and return array(vector)
void ReplaceAll( string& str, const string& find_str, const string& repl_str);
void printhex(string str);
void RemoveSpaces( char * instr);



// trim both ends

char *trim(char *str)
{
	ltrim(str);
	rtrim(str);
	return str;
}



// trim the left end only

char *ltrim(char *str)
{
	unsigned int i=0,j;

	if (strlen(str) == 0)
	{
		*str=0;
		return str;
	}

	if (!str) return NULL;	// handle NULL string
	if (!*str) return str;	// handle empty string


	while (str[i] <= 0x20)
	i++;
	// i now points to first non-whitespace char on left side

	if ( i > 0 )
	{
		// now move the string to the left
	for(j=0; j < strlen(str); j++)
	str[j]=str[j+i];
	str[j]='\0';
	}

	return str;
}



// trim whitspace from the right end

char *rtrim(char *str)
{
//	int i=0;
	char * ptr;

	if (strlen(str) == 0)
	{
		*str=0;
		return str;
	}

	if (!str) return NULL;	// handle NULL string
	if (!*str) return str;	// handle empty string

	for (ptr = str + strlen(str)-1;	(ptr >= str) && isspace(*ptr); --ptr);
	ptr[1]= '\0';

/*
	i=strlen(str)-1; // point to last char

	while (str[i]<=0x20)
	i--;

	// i now points to the first non-whitespace char at end of string
	if ( i<(strlen(str)-1) )
	str[i+1]='\0';
*/
	return str;
}



/*
	replace CR|LF with zzzzeroes
	if any case, add a zero to the end of string

*/

void chomp(char *str)
{
	unsigned int n;

	if (strlen(str) == 0)
	{
		*str=0;
		return;
	}

	for (n=0; n<strlen(str); n++)
	{
		switch(*(str+n))
		{
		case 0x0D:
			*(str+n)=0;
			break;
		case 0x0A:
			*(str+n)=0;
			break;
		}
	}

//	*(str+n)=0; // ensure ASCIZ even if no CRLF
}



/*
	Takes an ASCII HEX string as input (23dF93)
	converts it to a unit32_t binary
	NOTE: any "0x" chars must be removed first
*/


uint32_t htoi(char *ptr)
{
	uint32_t value=0;
	char ch =*ptr;

	while (ch == ' ' || ch == '\t')
		ch = *(++ptr);

	for (;;)
	{
		if (ch >= '0' && ch <= '9')
			value = (value << 4) + (ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			value = (value <<4) + (ch -'A' + 10);
		else if (ch >= 'a' && ch <= 'f')
			value = (value <<4) + (ch -'a' +10);
		else
			return value;
		ch = *(++ptr);

	}
}



/*
	convert a string to upper case (in place)
*/

void ucase(char *ptr)
{

	while ( *ptr != '\0')
	{
		*ptr = toupper(*ptr);
		ptr++;
	}

}



string ucase(string str)
{
	for (int n=0; n < str.size(); n++)
		str[n] = toupper(str[n]);

	return str;
}


/*
	convert a string to lower case (in place)
*/

void lcase(char *ptr)
{

	while ( *ptr != '\0')
	{
		*ptr = tolower(*ptr);
		ptr++;
	}

}




string lcase(string str)
{
	for (int n=0; n < str.size(); n++)
		str[n] = tolower(str[n]);

	return str;
}





// count the number of occurrences of mychar in mystring and return the count

int count_occurrences(string mystring, char mychar)
{

	int count=0;
	for (int n=0; n < mystring.size(); n++)
		if ( mystring[n] == mychar )	count++;


	return count;
}



/*
	split a string on delim and return an array of parsed out elements

	delim may contain several characters, all of which will be considered tokenizing
	delimeters and the src string will be broken apart on any of those delimeters


	EXAMPLE USAGE:

	string teststring="yes,no,maybe";
	vector<string> t = split(teststring,",");

	for (int xx=0; xx< t.size(); xx++)
		printf("%s\n",t[xx].c_str() );

	output:
			yes
			no
			maybe

	output with delim = ",b"
			yes
			no
			may
			e


*/

vector<string> split( const string &src, const string &delim)
{
	vector<string> result;

	string::size_type startPos=0, endPos=0;
	if (src.length() == 0) return result;

	do
	{
		endPos = src.find_first_of(delim,startPos);
		string::size_type length = endPos - startPos;
		if (length != 0)
			result.push_back (src.substr(startPos, length) );
		startPos = endPos +1;
	} while (endPos != string::npos);
	return result;
}


//#include <stdio.h>


/*
	ReplaceAll( string, find string, replacement string
	replace all occurrences of the "find" string with the "replacement" string

	EXAMPLE:
	string tbl="users devices";
    ReplaceAll( tbl, " ", "_");
	tbl now="users_devices";

*/

void ReplaceAll( string& str, const string& find_str, const string& repl_str)
{
//printf("src string: %s\r\n", str.c_str() );


	if (find_str.empty()) return;
	size_t start_pos = 0;

	while ( (start_pos = str.find(find_str, start_pos)) != string::npos)
	{
		str.replace(start_pos, find_str.length(), repl_str);
		start_pos += repl_str.length();	// in case "repl_str" contains "find_str", eg: replacing "x" with "yx"
	}

//printf("FIND: %s  REPL: %s\r\n", find_str.c_str(), repl_str.c_str() );
//printf("after: %s\r\n", str.c_str() );

}






void printhex(string str)
{
    unsigned int n;

    for (n=0; n< str.length(); n++)
        printf("%02X ",(unsigned char) str[n]);
}


// remove all spaces from a string in place
void RemoveSpaces( char * instr)
{
	int outptr=0;
	int length = strlen(instr)+1;
	char * outstr= new (nothrow) char[length];

	bzero(outstr,length);

	for (int n=0; n< strlen(instr); n++)
	{
		if (instr[n] != ' ')
		{
			outstr[outptr] = instr[n];
			outptr++;
			outstr[outptr]= (char) 0x0;	// ensure a terminator
		}
	}

	strcpy(instr,outstr);

	delete[] outstr;

}









