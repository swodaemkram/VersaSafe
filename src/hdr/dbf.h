
#include <mysql.h>

typedef struct
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	int port=4300;
	char server[50];
	char user[50];
	char password[50];
	char database[50];
} dbfinfo;


