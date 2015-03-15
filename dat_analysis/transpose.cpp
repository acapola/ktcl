 #include <tcl.h>
#include "tclchannelstreambuf.h"
#include "error_msg.h"
#include <sstream>
#include <bitset>
#include <stdio.h>
#include <string.h>
#include <fstream>
/*

std::ifstream::pos_type filesize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}
std::ifstream::pos_type filesize(std::ifstream in) {
    std::streampos gpos = in.tellg();
	in.seekg (0, std::ios::end);
    std::streampos end= in.tellg(); 
	in.seekg (gpos, std::ios::beg);
	return end;
}*/

long transpose(std::ifstream &in, std::ofstream &out, long total_length, long block_length) try {
	long cnt=0;
	char b;
	long nblocks = total_length/block_length;
	for(long i=0;i<block_length;i++) {
		in.seekg (i, in.beg);
		for(long block=0;block<nblocks;block++){
			in.get(b);
			//out.write(&b,1);
			out.put(b);
			in.seekg(block_length-1, in.cur);
			cnt++;
		}
	}
	return cnt;
} ERROR_MSG_CATCH("transpose(std::ifstream &in, std::ofstream &out, long total_length, long total_length, long block_length)")

long transpose(std::string in_file_name, std::string out_file_name, long total_length, long block_length) try {
	long cnt=0;
	std::ifstream is (in_file_name, std::ifstream::binary);
	std::ofstream out (out_file_name, std::ofstream::binary);
	cnt = transpose(is,out,total_length,block_length);
	is.close();
	out.close();
	return cnt;
} ERROR_MSG_CATCH("transpose(std::string in_file_name, std::string out_file_name, long total_length, long total_length, long block_length)")

long transpose(Tcl_Interp *interp, std::string in_file_name, std::string out_file_name, long total_length, long block_length) try {
	long cnt = transpose(in_file_name,out_file_name,total_length,block_length);
	return cnt;
} ERROR_MSG_CATCH("transpose(Tcl_Interp *interp, std::string in_file_name, std::string out_file_name, long total_length, long block_length)")

#define IN_CHAN_IDX 1
#define OUT_CHAN_IDX 2
#define TOTAL_LENGTH_IDX 3
#define BLOCK_LENGTH_IDX 4

 int transpose_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])  try {
	std::string in_file_name;
	std::string out_file_name;
	long total_length=-1;
	long block_length=-1;
	int mode;
	if(objc!=5) {
		throw error_msg("wrong # args: should be:\n   file_in file_out total_length block_length");
	}
	in_file_name = Tcl_GetString(objv[IN_CHAN_IDX]);
	out_file_name = Tcl_GetString(objv[OUT_CHAN_IDX]);
	//check the file system
	/*Tcl_Channel input_channel = Tcl_GetChannel(interp, in_file_name.c_str(), &mode);
	if (input_channel == (Tcl_Channel) NULL) throw error_msg("input_channel is null");
	if ((mode & TCL_READABLE) == 0) {
		std::stringstream msg;
		msg << "file \""<< in_file_name<<"\" cannot be opened for reading";
		throw error_msg(msg.str());
	}
	Tcl_Channel output_channel = Tcl_GetChannel(interp, out_file_name.c_str(), &mode);
	if (output_channel == (Tcl_Channel) NULL) throw error_msg("output_channel is null");
	if ((mode & TCL_WRITABLE) == 0) {
		std::stringstream msg;
		msg << "file \""<< out_file_name<<"\" cannot be opened for writing";
		throw error_msg(msg.str());
	}*/
	
	
	if (Tcl_GetLongFromObj(interp, objv[TOTAL_LENGTH_IDX], &total_length) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[TOTAL_LENGTH_IDX])<<"\" to long integer value";
		throw error_msg(msg.str());
	}
	if (Tcl_GetLongFromObj(interp, objv[BLOCK_LENGTH_IDX], &block_length) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[BLOCK_LENGTH_IDX])<<"\" to long integer value";
		throw error_msg(msg.str());
	}
	unsigned int cnt = transpose(interp,in_file_name,out_file_name,total_length,block_length);
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
