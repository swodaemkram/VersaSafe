#include <vector>
#include <string.h>
#include <string>

using namespace std;

extern char *trim(char *str);
extern char *ltrim(char *str);
extern char *rtrim(char *str);
extern void chomp(char *str);
extern uint32_t htoi(char *ptr);
extern void ucase(char *ptr);
extern void lcase(char *ptr);
extern string ucase(string str);
extern string lcase(string str);
extern int count_occurrences(string mystring, char mychar);
extern vector <string> split( const string &src, const string &delim);
extern void ReplaceAll( string& str, const string& find_str, const string& repl_str);
extern void printhex(string str);
extern void RemoveSpaces(char * instr);
