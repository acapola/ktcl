#include <stdlib.h>
#include <stdio.h>
#include <tcl.h>
#include <string.h>


#include <iostream>
#include <sstream>
#include <limits>
#include <iomanip>
//#include <algorithm>
//#include <vector>
#include <cctype>
#include <locale>
using namespace std;

#include "error_msg.h"
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

void evalCommand(Tcl_Interp *interp, const char *cmdStr){
	int code = Tcl_Eval(interp, cmdStr);
	if (code == TCL_OK) {
		Tcl_Channel chan = Tcl_GetStdChannel(TCL_STDOUT);
		if (chan) {
			Tcl_WriteObj(chan, Tcl_GetObjResult(interp));
			Tcl_WriteChars(chan, "\n", 1);
		}
	} else {
		Tcl_Channel chan = Tcl_GetStdChannel(TCL_STDERR);
		if (chan) {
			Tcl_WriteObj(chan, Tcl_GetObjResult(interp));
			Tcl_WriteChars(chan, "\n", 1);
		}
	}
}

int dat_analysis_init(Tcl_Interp *interp);

void test(void);
int main(int argc, char *argv[]) try {
	printf("ktcl\n");
	Tcl_FindExecutable(argv[0]);
    interp = Tcl_CreateInterp();
	
	dat_analysis_init(interp);
	//test();
	evalCommand(interp,"source ../test.tcl");
	
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
			evalCommand(interp,cmdStr);
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
catch(error_msg e)
{
	cout <<e.get_err_text()<<endl<<"Error code "<<e.get_err_code()<<endl;
	getchar();
	return e.get_err_code();
}
catch(...)
{
	cout <<"unknown exception caught !"<<endl;
	getchar();
	return -1;
}