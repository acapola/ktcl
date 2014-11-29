 #include <tcl.h>
#include "tclchannelstreambuf.h"
#include "error_msg.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
void dbgPrint(char *p){
	Tcl_Channel chan;
	chan = Tcl_GetStdChannel(TCL_STDOUT);
	if (chan != NULL) {
		Tcl_WriteChars(chan, p,strlen(p));
		Tcl_Flush(chan);
	} else {
		printf("can't get TCL_STDOUT");
	}
}

/*
proc rawToText { fin fout {maxLength 0} } {
	fconfigure $fin -translation binary
	set cnt 0
	while { 1 } {
		set bBin [read $fin 1]
		if {$bBin==""} {break}
		set bText ""
		binary scan $bBin b* bText
		puts -nonewline $fout "[string range $bText 1 7] "
		incr cnt
		if {$cnt%4==0} {puts -nonewline $fout "\n"}
		if {$cnt==$maxLength} {break}
	}
}
*/
/*
void test(void) try {
	throw error_msg("test exception");
} ERROR_MSG_CATCH("test(void)")
*/
typedef unsigned char uint8_t;
long raw_to_text(std::istream &in, std::ostream &out, long max_length) try {
	long cnt=0;
	char b;
	if(-1==max_length) max_length = LONG_MAX;
	while((cnt<max_length) && in.get(b)){
		out.write(&b,1);
		cnt++;
	}
	return cnt;
} ERROR_MSG_CATCH("raw_to_text(std::istream &in, std::ostream &out, int max_length)")

#include <fstream>
void test(void){
	std::ofstream out;
	out.open ("c:\\tmp\\test.out.txt");
	std::ifstream in ("c:\\tmp\\test.dat");
	raw_to_text(in,out,-1);
	out.close();
	in.close();
}

long raw_to_text(Tcl_Interp *interp, Tcl_Channel input_channel,	Tcl_Channel output_channel, long max_length) try {
	if(TCL_OK!=Tcl_SetChannelOption(interp, input_channel, "-translation", "binary")) {//use the channel in binary mode
		int errorCode = Tcl_GetErrno();
		//const char *machineReadableMessage = Tcl_ErrnoId();
		const char *hummanReadableMessage = Tcl_ErrnoMsg(errorCode);
		throw error_msg(errorCode,hummanReadableMessage);
	}
	tclchannel_istream in(input_channel);
	tclchannel_ostream out(output_channel);
	long cnt = raw_to_text(in,out,max_length);
	out.flush();
	return cnt;
} ERROR_MSG_CATCH("raw_to_text(Tcl_Interp *interp, Tcl_Channel input_channel,	Tcl_Channel output_channel, int max_length)")

#define IN_CHAN_IDX 1
#define OUT_CHAN_IDX 2
#define MAX_LENGTH_IDX 3

 static int raw_to_text_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])  try {
	Tcl_Channel input_channel;
	Tcl_Channel output_channel;
	long max_length;
	int mode;
	
	if(objc!=4) {
		throw error_msg("wrong # args: should be:\n   file_in file_out ?max_length?");
	}
	input_channel = Tcl_GetChannel(interp, Tcl_GetString(objv[IN_CHAN_IDX]), &mode);
	if (input_channel == (Tcl_Channel) NULL) {
		return TCL_ERROR;
	}
	if ((mode & TCL_READABLE) == 0) {
		Tcl_AppendResult(interp, "channel \"", Tcl_GetString(objv[IN_CHAN_IDX]),
			"\" wasn't opened for reading", (char *) NULL);
		return TCL_ERROR;
	}
	
	output_channel = Tcl_GetChannel(interp, Tcl_GetString(objv[OUT_CHAN_IDX]), &mode);
	if (output_channel == (Tcl_Channel) NULL) {
		return TCL_ERROR;
	}
	if ((mode & TCL_WRITABLE) == 0) {
		Tcl_AppendResult(interp, "channel \"", Tcl_GetString(objv[OUT_CHAN_IDX]),
			"\" wasn't opened for writing", (char *) NULL);
		return TCL_ERROR;
	}
	
	if (Tcl_GetLongFromObj(interp, objv[MAX_LENGTH_IDX], &max_length) != TCL_OK) {
		return TCL_ERROR;
	}
	unsigned int cnt = raw_to_text(interp,input_channel,output_channel,max_length);
	Tcl_SetObjResult(interp, Tcl_NewLongObj(cnt));
	//Tcl_SetObjResult(interp, Tcl_NewStringObj("Hello, World!", -1));
	return TCL_OK;
} catch(error_msg e){
	std::stringstream msg;
	msg<<e.get_err_text()<<std::endl<<"Error code "<<e.get_err_code()<<std::endl;
	Tcl_AppendResult (interp, "\nERROR: ",Tcl_GetString(objv[0]), msg.str().c_str(), (char *) NULL);
	return TCL_ERROR;
} catch(...){
	Tcl_AppendResult (interp, "\nERROR: ",Tcl_GetString(objv[0]), "unknown exception caught !", (char *) NULL);
	return TCL_ERROR;
}

int /*DLLEXPORT*/ raw_to_text_init(Tcl_Interp *interp) {
        Tcl_Namespace *nsPtr; /* pointer to hold our own new namespace */

        if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
                return TCL_ERROR;
        }
        
      /* create the namespace named 'hello' */
        nsPtr = Tcl_CreateNamespace(interp, "dat_analysis", NULL, NULL);
        if (nsPtr == NULL) {
            return TCL_ERROR;
        }
        
      /* just prepend the namespace to the name of the command.
         Tcl will now create the 'hello' command in the 'hello'
         namespace so it can be called as 'hello::hello' */
        Tcl_CreateObjCommand(interp, "dat_analysis::raw_to_text", raw_to_text_cmd, NULL, NULL);
        Tcl_PkgProvide(interp, "dat_analysis", "1.0");
        return TCL_OK;
 }
 