#include "hdr/dbf.h"


extern int GetRow(dbfinfo *dbf);
extern int QueryDBF(dbfinfo *dbf, char * query);
extern void InitDBF(dbfinfo *dbf);
bool ConnectDBF(dbfinfo *dbf);



extern dbfinfo localDBF;
extern dbfinfo remoteDBF;
