/*

	Module: dbf.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 4-1-2012
    Revised: 3-25-2013
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2018 FireKing Secrutiy
    Version: 1.0


	This module handles the database I/O and ancillary functions.
	The module allows connecting to a MySql server, either remotely or local

	DBF TABLES
	========================================================

	Filename			Structure			Description
	--------			---------------		---------------------------------------------
	controller_x.sdf	actuator_struc		holds all information about all actuators for a controller
		(where x is the maestro/controller number (0-9)

*/


using namespace std;

#define MB_DEBUG

#define BUFF_LEN 1000


#ifndef FIRE_INCLUDE
	#include "hdr/fire.h"
#endif


#include "logs.inc"
#include "common.inc"
#include "hdr/dbf.h"

#include <iostream>
#include "hdr/config.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


/*
// mysql stuff
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "mysql_connection.h"
*/

#include <mysql.h>
#include <errmsg.h>


// PUBLIC DECLARATIONS
bool ConnectDBF(dbfinfo *dbf);
bool CloseDBF(void);
int QueryDBF(dbfinfo *dbf, char * query);
int GetRow(dbfinfo *dbf);
void InitDBF(dbfinfo *dbf);


// EXTERNALS
extern char *trim(char *str);
extern char *ltrim(char *str);
extern char *rtrim(char *str);
extern void chomp(char *str);

extern char gen_buffer[];	// general purpose buffer, use and lose (in common.cpp)


// INTERNALS




dbfinfo localDBF;
dbfinfo remoteDBF;


/*
	/etc/mysql/mysql.conf.d# nano mysqld.cnf

	added the "port=4300" line in above file

*/

void InitDBF(dbfinfo *dbf)
{
	if (dbf == &localDBF)
	{

		strcpy(dbf->server,"localhost");
		strcpy(dbf->user,"fkinguser101");
		strcpy(dbf->password,"glimpbust345");
		strcpy(dbf->database,"fking");
		dbf->port=4300;	// 3306
	}
	if (dbf == &remoteDBF)
	{
	}
}



bool ConnectDBF(dbfinfo *dbf)
{

	dbf->conn= mysql_init(NULL);

printf("USER: %s\n",dbf->user);
/*
MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *password, const char *user, const char *password, const char *database, uint port, const char *user, const char *password, const char *database, uint port, const char *unix_socket, uint flag)
*/
   /* Connect to database */
   if (!mysql_real_connect(dbf->conn, dbf->server,
         dbf->user, dbf->password, dbf->database, dbf->port, NULL, 0)) 
	{
	fprintf(stderr, "%s\n", mysql_error(dbf->conn));
	return false;
	}

   /* send SQL query */
   if (mysql_query(dbf->conn, "show tables")) {
      fprintf(stderr, "%s\n", mysql_error(dbf->conn));
	return false;
   }

//   res = mysql_use_result(conn);
	dbf->res = mysql_store_result(dbf->conn);

//int i=0;
//unsigned int numrows = mysql_num_rows(res);
//printf("Rows: %d\n",numrows);
/*
   // output table name 
   printf("MySQL Tables in mysql database:\n");
   while ((row = mysql_fetch_row(res)) != NULL)
      printf("%s \n", row[0]);
*/

   /* close connection */
   mysql_free_result(dbf->res);



	return true;


}



bool CloseDBF()
{
	mysql_close(localDBF.conn);
	return true;
}



/*
	read a row from the database per the last query
	row[0] = data fronm first requested field
	row[1] = data from second requested field

	RETURN:	number of rows(which may be zero), or -1 on error
*/

int GetRow(dbfinfo *dbf)
{
	if (dbf->res == NULL)
	{
		sprintf(gen_buffer,"%s\n",mysql_error(dbf->conn));
		WriteSystemLog(gen_buffer);
		return -1;
	}

	int numrows = mysql_num_rows(dbf->res);
	printf("Rows: %d\n",numrows);

	dbf->row = mysql_fetch_row(dbf->res);
	return numrows;
}



/*
	query the MySQl server and return the status
	also, fill in the res entry for the caller
	returns 0 on success, nonzero on err
*/

int QueryDBF(dbfinfo *dbf, char * query)
{
	int result;
	// send SQL query
	// returns 0 on success
	//int mysql_query(MYSQL *mysql, const char *stmt_str)

	result = mysql_query(dbf->conn, query);
	char tstamp[50];
	MakeTimeStamp(tstamp);

	dbf->res=NULL;	// preset to error

	switch(result)
	{
	case CR_COMMANDS_OUT_OF_SYNC:
		sprintf(gen_buffer,"MYSQL ERR: %s",mysql_error(dbf->conn));
		WriteSystemLog(gen_buffer);
		break;
	case CR_SERVER_GONE_ERROR:
		sprintf(gen_buffer,"MYSQL ERR: %s",mysql_error(dbf->conn));
		WriteSystemLog(gen_buffer);
		break;
	case CR_SERVER_LOST:
		sprintf(gen_buffer,"MYSQL ERR: %s",mysql_error(dbf->conn));
		WriteSystemLog(gen_buffer);
		break;
	default:
	case CR_UNKNOWN_ERROR:
		sprintf(gen_buffer,"MYSQL ERR: %s",mysql_error(dbf->conn));
		WriteSystemLog(gen_buffer);
		break;
	case 0:		// success
		dbf->res = mysql_store_result(dbf->conn);
		sprintf(gen_buffer,"Success");
		printf("<DBFRES:%p>\n",dbf->res);
		break;
	}

	printf("gen_buffer: %s:: %d\n",gen_buffer, result);
	return result;


}





//EOM


