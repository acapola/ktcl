#include <stdlib.h>
#include <stdio.h>
#include <tcl.h>
#include <string.h>

Tcl_Interp * interp ;

#define DEFAULT_PRIMARY_PROMPT	  "ktcl% "
#define DEFAULT_INCOMPLETE_PROMPT ">     "
void prompt(bool newCmd){
	const char *p = newCmd ? DEFAULT_PRIMARY_PROMPT : DEFAULT_INCOMPLETE_PROMPT;
	Tcl_Channel chan;
	chan = Tcl_GetStdChannel(TCL_STDOUT);
	if (chan != NULL) {
		Tcl_WriteChars(chan, p,strlen(p));
		Tcl_Flush(chan);
	} else {
		printf("can't get TCL_STDOUT");
	}
}

int main(int argc, char *argv[]) {
	int code;
	printf("ktcl\n");
	Tcl_FindExecutable(argv[0]);
    interp = Tcl_CreateInterp();
	Tcl_Channel chanIn = Tcl_GetStdChannel(TCL_STDIN);
	Tcl_DString cmd;
	Tcl_DStringInit(&cmd);
	Tcl_DString line;
	Tcl_DStringInit(&line);
	bool newCmd = true;
	do{
		prompt(newCmd);
		Tcl_Gets(chanIn,&line);
		Tcl_DStringAppend(&cmd,Tcl_DStringValue(&line),-1);
		const char *cmdStr = Tcl_DStringValue(&cmd);
		newCmd = Tcl_CommandComplete(cmdStr);
		if(newCmd){
			code = Tcl_Eval(interp, cmdStr);
			if (code != TCL_OK) {
				Tcl_Channel chan = Tcl_GetStdChannel(TCL_STDERR);
				if (chan) {
					Tcl_WriteObj(chan, Tcl_GetObjResult(interp));
					Tcl_WriteChars(chan, "\n", 1);
				}
			}
			Tcl_DStringTrunc(&cmd, 0);
		} else {
			//printf("command incomplete: %s\n",cmdStr);
		}
		Tcl_DStringTrunc(&line, 0);
	} while(1);
	Tcl_DStringFree(&line);
	Tcl_DStringFree(&cmd);
	return 0;
}
