#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "commonTypesLib.h"

#define CALLBACK

typedef void *PDEBUG_CLIENT;
typedef bool (CALLBACK *DBGFUNC) (PDEBUG_CLIENT Client, char* args);

typedef struct _DBGEXTCOMMAND
{
    char*    szCommandName;
    DBGFUNC  pfDBGFunc;
} DBGEXTCOMMAND, *PDBGEXTCOMMAND;

bool CALLBACK ghelp(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gasiclist(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gs(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gdisplay(PDEBUG_CLIENT Client, char* args);
bool CALLBACK glistpci(PDEBUG_CLIENT Client, char* args);
bool CALLBACK grbdecode(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gibdecode(PDEBUG_CLIENT Client, char* args);
bool CALLBACK grbscan(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gibscan(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gemulate(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gwalk(PDEBUG_CLIENT Client, char* args);
bool CALLBACK glook(PDEBUG_CLIENT Client, char* args);
bool CALLBACK glookup(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gedit(PDEBUG_CLIENT Client, char* args);
bool CALLBACK geditf(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gdd(PDEBUG_CLIENT Client, char* args);
bool CALLBACK ged(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gds(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gloaddata(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gwritedata(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gdebug(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsgpr(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gvgpr(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gbmp(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsettings(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsurfdesc(PDEBUG_CLIENT Client, char* args);
bool CALLBACK greadindex(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gwriteindex(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsqind(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gcpq(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gfill(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gfind(PDEBUG_CLIENT Client, char* args);
bool CALLBACK ghwinfo(PDEBUG_CLIENT Client, char* args);
bool CALLBACK ghwrefresh(PDEBUG_CLIENT Client, char* args);
bool CALLBACK ghwstate(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gmqd(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gdiag(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gpagetable(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsrd(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsecurity(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gbitfields(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gcounter(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gsh(PDEBUG_CLIENT Client, char* args);
bool CALLBACK giommu(PDEBUG_CLIENT Client, char* args);
//macosx specific
bool CALLBACK gdmllink(PDEBUG_CLIENT Client, char* args);
bool CALLBACK gver(PDEBUG_CLIENT Client, char* args);
bool CALLBACK quit(PDEBUG_CLIENT Client, char* args);

#endif // __CONSOLE_H__
