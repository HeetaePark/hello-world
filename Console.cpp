#include <stdio.h>
#include <iostream>
#include <readline/readline.h>
#include <IOKit/IOKitLib.h>

#include "Common.h"
#include "Console.h"

#include "Platform.h"
#include "MacOSXExt.h"
#include "PlatformMacOSX.h"
#include "CmdLineArgs.h"
#include "ExtSettings.h"

#include "DebugExt.h"
#include "DiagDriver.h"

#define OSX_EXT_VERSION "test"

bool createDebugExt();

DBGEXTCOMMAND ConsoleCommands[] =
{
    ////////////////////////////////////////////////////
    // common Extension commands
    ////////////////////////////////////////////////////
    { (char*)"ghelp", &ghelp},
    { (char*)"gasiclist", &gasiclist},
    { (char*)"gs", &gs},
    { (char*)"gdisplay", &gdisplay},
    { (char*)"glistpci", &glistpci},
    { (char*)"grbdecode", &grbdecode},
    { (char*)"gibdecode", &gibdecode},
    { (char*)"grbscan", &grbscan},
    { (char*)"gibscan", &gibscan},
    { (char*)"gemulate", &gemulate},
    { (char*)"gwalk", &gwalk},
    { (char*)"glook", &glook},
    { (char*)"glookup", &glookup},
    { (char*)"gedit", &gedit},
    { (char*)"geditf", &geditf},
    { (char*)"gdd", &gdd},
    { (char*)"ged", &ged},
    { (char*)"gds", &gds},
    { (char*)"gwritedata", &gwritedata},
    { (char*)"gloaddata", &gloaddata},
    { (char*)"gdebug", &gdebug},
    { (char*)"gsgpr", &gsgpr},
    { (char*)"gvgpr", &gvgpr},
    { (char*)"gbmp", &gbmp},
    { (char*)"gsettings", &gsettings},
    { (char*)"gsurfdesc", &gsurfdesc},
    { (char*)"greadindex", &greadindex},
    { (char*)"gwriteindex", &gwriteindex},
    { (char*)"gsqind", &gsqind},
    { (char*)"gcpq", &gcpq},
    { (char*)"gfill", &gfill},
    { (char*)"gfind", &gfind},
    { (char*)"ghwinfo", &ghwinfo},
    { (char*)"ghwrefresh", &ghwrefresh},
    { (char*)"ghwstate", &ghwstate},
    { (char*)"gmqd", &gmqd},
    { (char*)"gdiag", &gdiag},
    { (char*)"gpagetable", &gpagetable},
    { (char*)"gsrd", &gsrd},
    { (char*)"gsecurity", &gsecurity},
    { (char*)"gbitfields", &gbitfields},
    { (char*)"gcounter", &gcounter},
    { (char*)"gsh", &gsh},
    { (char*)"giommu", &giommu},
    
    
    /////////////////////////////////////////////////////
    // MacOSX specific commands
    /////////////////////////////////////////////////////
    { (char*)"gdmllink", &gdmllink},
    { (char*)"gver", &gver},
    { (char*)"quit", &quit},
};

bool execCommand(char *szCommand, char *szLastCommand, size_t buf_size)
{
    bool bValidCommand = false;
    unsigned int i,k;
    
    if (strlen(szCommand))
    {
        if (szCommand[strlen(szCommand)-1] == '\n')
        {
            szCommand[strlen(szCommand)-1] = 0;
        }
        strcpy(szLastCommand, szCommand);
    }
    else
    {
        strcpy(szCommand, szLastCommand);
    }
    
    for (i=0; i < sizeof(ConsoleCommands)/sizeof(DBGEXTCOMMAND); i++)
    {
        if (strncasecmp(szCommand, ConsoleCommands[i].szCommandName, strlen(ConsoleCommands[i].szCommandName)) == 0)
        {
            // We need to check if we found the full command to eliminate cases
            // when e.g. we will find "gcb" in "gcb_ibs" command.
            // For this reason check if we have nonspace symbol after.
            size_t len = strlen(ConsoleCommands[i].szCommandName);
            
            if (!(isspace(szCommand[len]) || szCommand[len] == '\0')) continue;
            
            k = 0;
            
            while (szCommand[k] == ConsoleCommands[i].szCommandName[k])
            {
                k++;
            }
            
            while(szCommand[k] == ' ')
            {
                k++;
            }
            
            (ConsoleCommands[i].pfDBGFunc) (NULL, &szCommand[k]);
            bValidCommand = true;
            break;
        }
    }
    
    return bValidCommand;
}

int main(int argc, const char *argv[])
{
    
    char szCommand[200];
    char szLastCommand[200];
    char *pInput;
    
    SCAN_AMDDIAG result = DiagDriver::searchAmdGpuIoService();
    
    if (result == NOT_FOUND_AMDDIAG_CLASS)
    {
        printf("Error: can't find AMDDiag Class. If amddiag.kext is not loaded, load latest version from torcamacsrv like\n");
        printf("       \x1b[36msmb://torcamacsrv/Powerpc_all/Software Release/Software Component Drop/AMDDiag/Golden/1.2.5/AMDDiagGoldenVer1.2.5-Jul-22-17-58-17.pax\x1b[0m\n");
        return EXIT_FAILURE;
    }
    else if (result == NOT_FOUND_AMD_GPU)
    {
        printf("Error: can't find supporting AMD GPU, check if there is CI or later GPUs\n");
        return EXIT_FAILURE;
    }
    
    if (!createDebugExt())
    {
        printf("Error: can't create DebugExt objext\n");
        return EXIT_FAILURE;
    }
    
    DebugExt* pDebugExt = MacOSXExt::GetMacOSXExt()->GetDebugExt();
    pDebugExt->DiscoverAmdGraphicsAdapters();
    MacOSXExt::GetMacOSXExt()->NotifySessionAccessState(true);
    
    using_history();
    
    do
    {
        bool bValidCommand = false;
        
        pInput = readline("AEXT>");
        
        if (!pInput)
        {
            exit(1);
        }
        
        add_history(pInput);
        memset(szCommand,0,200);
        strcpy(szCommand,pInput);
        free(pInput);
        
        bValidCommand = execCommand(szCommand, szLastCommand, 200);
        
        if (!bValidCommand)
        {
            printf("Invalid Command - Enter help for more info\n");
        }
        
    } while (strncasecmp(szCommand,"quit",4) != 0);
    
    DiagDriver::closeIoService();
    
    return EXIT_SUCCESS;
}

bool createDebugExt()
{
    bool result = false;

    Platform* pPlaform =  Platform::CreatePlatform();
    DebugExt* pDebugExt = DebugExt::CreateDebugExt(pPlaform);
    
    if (pDebugExt)
    {
        MacOSXExt* pMacOSXExt = MacOSXExt::GetMacOSXExt();
        if (pMacOSXExt)
        {
            printf("-----------------------------------------------------\n");
            printf("Debug Extension Console " OSX_EXT_VERSION "\n");
            printf("-----------------------------------------------------\n");
            result = true;
        }
    }
    
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// DebugExt commmands start from here
/////////////////////////////////////////////////////////////////////////////////////////////
bool CALLBACK ghelp(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("ghelp", client, args) ? true : false;
}

bool CALLBACK gasiclist(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gasiclist", client, args) ? true : false;
}

bool CALLBACK gs(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gs", client, args) ? true : false;
}

bool CALLBACK gdisplay(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gdisplay", client, args) ? true : false;
}

bool CALLBACK glistpci(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("glistpci", client, args) ? true : false;
}

bool CALLBACK grbdecode(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("grbdecode", client, args) ? true : false;
}

bool CALLBACK gibdecode(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gibdecode", client, args) ? true : false;
}

bool CALLBACK grbscan(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("grbscan", client, args) ? true : false;
}

bool CALLBACK gibscan(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gibscan", client, args) ? true : false;
}

bool CALLBACK gemulate(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gemulate", client, args) ? true : false;
}

bool CALLBACK gwalk(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gwalk", client, args) ? true : false;
}

bool CALLBACK glook(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("glook", client, args) ? true : false;
}

bool CALLBACK glookup(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("glookup", client, args) ? true : false;
}

bool CALLBACK gedit(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gedit", client, args) ? true : false;
}

bool CALLBACK geditf(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("geditf", client, args) ? true : false;
}

bool CALLBACK gdd(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gdd", client, args) ? true : false;
}

bool CALLBACK ged(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("ged", client, args) ? true : false;
}

bool CALLBACK gds(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gds", client, args) ? true : false;
}

bool CALLBACK gwritedata(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gwritedata", client, args) ? true : false;
}

bool CALLBACK gloaddata(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gloaddata", client, args) ? true : false;
}

bool CALLBACK gdebug(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gdebug", client, args) ? true : false;
}

bool CALLBACK gsgpr(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsgpr", client, args) ? true : false;
}

bool CALLBACK gvgpr(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gvgpr", client, args) ? true : false;
}

bool CALLBACK gbmp(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gbmp", client, args) ? true : false;
}

bool CALLBACK gsettings(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsettings", client, args) ? true : false;
}

bool CALLBACK gsurfdesc(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsurfdesc", client, args) ? true : false;
}

bool CALLBACK greadindex(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("greadindex", client, args) ? true : false;
}

bool CALLBACK gwriteindex(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gwriteindex", client, args) ? true : false;
}

bool CALLBACK gsqind(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsqind", client, args) ? true : false;
}

bool CALLBACK gcpq(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gcpq", client, args) ? true : false;
}

bool CALLBACK gfill(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gfill", client, args) ? true : false;
}

bool CALLBACK gfind(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gfind", client, args) ? true : false;
}

bool CALLBACK ghwinfo(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("ghwinfo", client, args) ? true : false;
}

bool CALLBACK ghwrefresh(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("ghwrefresh", client, args) ? true : false;
}

bool CALLBACK ghwstate(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("ghwstate", client, args) ? true : false;
}

bool CALLBACK gmqd(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gmqd", client, args) ? true : false;
}

bool CALLBACK gdiag(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gdiag", client, args) ? true : false;
}

bool CALLBACK gpagetable(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gpagetable", client, args) ? true : false;
}

bool CALLBACK gsrd(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsrd", client, args) ? true : false;
}

bool CALLBACK gsecurity(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsecurity", client, args) ? true : false;
}

bool CALLBACK gbitfields(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gbitfields", client, args) ? true : false;
}

bool CALLBACK gcounter(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gcounter", client, args) ? true : false;
}

bool CALLBACK gsh(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("gsh", client, args) ? true : false;
}

bool CALLBACK giommu(PDEBUG_CLIENT client, char* args)
{
    return MacOSXExt::GetMacOSXExt()->DispatchCommand("giommu", client, args) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool CALLBACK gdmllink(PDEBUG_CLIENT Client, char* args)
{
    bool enable;
    PlatformMacOSX *platform = NULL;
    
    if (!strcmp(args, "1")) enable = true;
    else if (!strcmp(args, "0")) enable = false;
    else return true;
    
    platform = (PlatformMacOSX *)MacOSXExt::GetMacOSXExt()->GetPlatform();
    if (platform)
    {
        platform->setShowDmlLink(enable);
        printf("Showing DML Link is %s\n", enable?"enalbed":"disabled");
    }
    return true;
}

bool CALLBACK quit(PDEBUG_CLIENT Client, char* args)
{
    return true;
}

bool CALLBACK gver(PDEBUG_CLIENT Client, char* args)
{
    printf("Debug Extension Console " OSX_EXT_VERSION "\n");
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// DebugExt commmands ends here
/////////////////////////////////////////////////////////////////////////////////////////////

