 #include <tcl.h>
#include "tclchannelstreambuf.h"
#include "error_msg.h"
#include <sstream>
#include <bitset>
#include <stdio.h>
#include <string.h>

long raw_to_text(std::istream &in, std::ostream &out, long max_length) try {
	long cnt=0;
	char b;
	if(-1==max_length) max_length = LONG_MAX;
	while((cnt<max_length) && in.get(b)){
		std::bitset<7> x(b>>1);
		out << x << " ";
		cnt++;
		if(0==cnt%4) out<<"\n";
	}
	return cnt;
} ERROR_MSG_CATCH("raw_to_text(std::istream &in, std::ostream &out, int max_length)")

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

 int raw_to_text_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])  try {
	Tcl_Channel input_channel;
	Tcl_Channel output_channel;
	long max_length=-1;
	int mode;
	if((objc<3) || (objc>4)) {
		throw error_msg("wrong # args: should be:\n   file_in file_out ?max_length?");
	}
	input_channel = Tcl_GetChannel(interp, Tcl_GetString(objv[IN_CHAN_IDX]), &mode);
	if (input_channel == (Tcl_Channel) NULL) throw error_msg("input_channel is null");
	if ((mode & TCL_READABLE) == 0) {
		std::stringstream msg;
		msg << "channel \""<< Tcl_GetString(objv[IN_CHAN_IDX])<<"\" wasn't opened for reading";
		throw error_msg(msg.str());
	}
	output_channel = Tcl_GetChannel(interp, Tcl_GetString(objv[OUT_CHAN_IDX]), &mode);
	if (output_channel == (Tcl_Channel) NULL) throw error_msg("output_channel is null");
	if ((mode & TCL_WRITABLE) == 0) {
		std::stringstream msg;
		msg << "channel \""<< Tcl_GetString(objv[OUT_CHAN_IDX])<<"\" wasn't opened for writing";
		throw error_msg(msg.str());
	}
	if(objc==4) {
		if (Tcl_GetLongFromObj(interp, objv[MAX_LENGTH_IDX], &max_length) != TCL_OK) {
			std::stringstream msg;
			msg << "cannot convert \""<< Tcl_GetString(objv[MAX_LENGTH_IDX])<<"\" to long integer value";
			throw error_msg(msg.str());
		}
	}
	unsigned int cnt = raw_to_text(interp,input_channel,output_channel,max_length);
	Tcl_SetObjResult(interp, Tcl_NewLongObj(cnt));
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
