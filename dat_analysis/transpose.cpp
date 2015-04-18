 #include <tcl.h>
#include "tclchannelstreambuf.h"
#include "error_msg.h"
#include <sstream>
#include <bitset>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <exception>
#include <stdexcept>
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
} ERROR_MSG_CATCH("transpose(std::ifstream &in, std::ofstream &out, long total_length, long block_length)")

long transpose(std::ifstream &in, std::ofstream &out, long total_length, long block_length, unsigned int chunk_width_in_bits) try {
	long cnt=0;
	char b;
	long nblocks = total_length/block_length;
	if((block_length*8)%chunk_width_in_bits) throw std::runtime_error("chunk_width_bits must divide block_length*8");
	long chunks_per_block = (block_length*8) / chunk_width_in_bits;
	char out_buf;
	unsigned int output_level=0;
	for(long chunk=0;chunk<chunks_per_block;chunk++) {
		for(long block=0;block<nblocks;block++){
			long in_offset_bits = block*block_length*8+chunk*chunk_width_in_bits;
			long in_offset = in_offset_bits /8;//rounding down
			in.seekg (in_offset, in.beg);
			unsigned int remaining = chunk_width_in_bits;
			while(remaining){
				//printf("read at: %d ",in.tellg());
				in.get(b);
				//printf("%02x, ",0x0FF & b);
				unsigned int in_nbits = 8-(in_offset_bits%8);
				unsigned int chunk_nbits;
				if(in_nbits >= chunk_width_in_bits) chunk_nbits = chunk_width_in_bits;
				else chunk_nbits = in_nbits;
				remaining -= chunk_nbits;
				b=b>>(in_offset_bits%8);
				unsigned char chunk_bits = b & (0x0FF >> (8-chunk_nbits));//force the unused bits to 0
				out_buf = out_buf | (chunk_bits<<output_level);
				unsigned int new_output_level = output_level + chunk_nbits;
				//printf("new_output_level = %d\n",new_output_level);
				if(new_output_level >= 8) {
					new_output_level -= 8;
					//printf("write: %02x\n",0x0FF & out_buf);
					out.put(out_buf);
					cnt++;
					out_buf = b>>(8-output_level);
					out_buf &= 0x0FF >> (8-new_output_level);//force the undefined bits to 0
				}
				output_level = new_output_level;
			}
		}
	}
	return cnt;
} ERROR_MSG_CATCH("transpose_chunks(std::ifstream &in, std::ofstream &out, long total_length, long block_length, unsigned int chunk_width_in_bits)")

long transpose(std::string in_file_name, std::string out_file_name, long total_length, long block_length) try {
	long cnt=0;
	std::ifstream is (in_file_name, std::ifstream::binary);
	std::ofstream out (out_file_name, std::ofstream::binary);
	cnt = transpose(is,out,total_length,block_length);
	is.close();
	out.close();
	return cnt;
} ERROR_MSG_CATCH("transpose(std::string in_file_name, std::string out_file_name, long total_length, long block_length)")

long transpose(std::string in_file_name, std::string out_file_name, long total_length, long block_length, unsigned int chunk_width_in_bits) try {
	long cnt=0;
	std::ifstream is (in_file_name, std::ifstream::binary);
	std::ofstream out (out_file_name, std::ofstream::binary);
	cnt = transpose(is,out,total_length,block_length,chunk_width_in_bits);
	is.close();
	out.close();
	return cnt;
} ERROR_MSG_CATCH("transpose(std::string in_file_name, std::string out_file_name, long total_length, long block_length, unsigned int chunk_width_in_bits)")

long transpose(Tcl_Interp *interp, std::string in_file_name, std::string out_file_name, long total_length, long block_length) try {
	long cnt = transpose(in_file_name,out_file_name,total_length,block_length);
	return cnt;
} ERROR_MSG_CATCH("transpose(Tcl_Interp *interp, std::string in_file_name, std::string out_file_name, long total_length, long block_length)")

long transpose(Tcl_Interp *interp, std::string in_file_name, std::string out_file_name, long total_length, long block_length, unsigned int chunk_width_in_bits) try {
	long cnt = transpose(in_file_name,out_file_name,total_length,block_length,chunk_width_in_bits);
	return cnt;
} ERROR_MSG_CATCH("transpose(Tcl_Interp *interp, std::string in_file_name, std::string out_file_name, long total_length, long block_length, unsigned int chunk_width_in_bits)")

#define IN_CHAN_IDX 1
#define OUT_CHAN_IDX 2
#define TOTAL_LENGTH_IDX 3
#define BLOCK_LENGTH_IDX 4
#define CHUNK_WIDTH_IDX 5

 int transpose_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])  try {
	std::string in_file_name;
	std::string out_file_name;
	long total_length=-1;
	long block_length=-1;
	int chunk_width_in_bits=0;
	//int mode;
	if((objc<5)&&(objc>6)) {
		throw error_msg("wrong # args: should be:\n   file_in file_out total_length block_length [chunk_width_in_bits]");
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
	unsigned int cnt;
	if (objc>CHUNK_WIDTH_IDX){
		if (Tcl_GetIntFromObj(interp, objv[CHUNK_WIDTH_IDX], &chunk_width_in_bits) != TCL_OK) {
			std::stringstream msg;
			msg << "cannot convert \""<< Tcl_GetString(objv[CHUNK_WIDTH_IDX])<<"\" to integer value";
			throw error_msg(msg.str());
		}
		if(chunk_width_in_bits<=0) throw error_msg("chunk_width_in_bits must be >= 0.");
		cnt = transpose(interp,in_file_name,out_file_name,total_length,block_length,(unsigned int) chunk_width_in_bits);
	} else {
		cnt = transpose(interp,in_file_name,out_file_name,total_length,block_length);
	}
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
