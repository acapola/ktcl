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
#include "boost/multi_array.hpp"
#include "boost/multiprecision/cpp_int.hpp"

long split(std::ifstream &in, long start_offset, long total_length, unsigned int block_width_in_bits, unsigned int chunk_width_in_bits, std::vector<std::string> out_names) try {
	long cnt=0;
	char b;
	unsigned int nchunks = block_width_in_bits / chunk_width_in_bits;
	if(out_names.size()!=nchunks) throw new std::runtime_error("out_names.size()!=nchunks");
	std::vector<std::ofstream> outs(nchunks);
	std::vector<unsigned int> chunk_out_buf(nchunks);
	std::vector<unsigned int> chunk_out_buf_level(nchunks);
	for(int chunk=0;chunk<nchunks;chunk++) {
		if(!out_names[chunk].empty()){
			outs[chunk].open(out_names[chunk], std::ofstream::binary);//TODO: check if the file is not already openned, then just use the  same ofstream (allow some kind of merge of outputs)
		}
	}
	long chunk_max_val = (1L<<chunk_width_in_bits)-1;
	long nblocks = (total_length*8) / block_width_in_bits;
	in.seekg (start_offset, in.beg);
	
	if(chunk_width_in_bits<=32){//up to 32bit chunks, use native integers as it is much faster than multiprecision
		for(long block=0;block<nblocks;block++){
			unsigned int chunk_val=0;
			unsigned long out_buf=0;
			unsigned int output_level=0;
			for(int chunk=0;chunk<nchunks;chunk++) {
				while(output_level<chunk_width_in_bits){
					in.get(b);
					unsigned char new_bits = 0x0FF & b;
					out_buf = out_buf | (new_bits<<output_level);
					output_level+=8;
				}
				output_level-=chunk_width_in_bits;
				chunk_val = out_buf & chunk_max_val;
				out_buf = out_buf >> chunk_width_in_bits;
				if(!out_names[chunk].empty()){//write to file
					unsigned int c_level = chunk_out_buf_level[chunk];
					unsigned int remaining = chunk_width_in_bits;
					if(c_level){
						char c = chunk_out_buf[chunk];
						remaining += c_level;
						if(remaining>=8){
							char to_write = c;
							chunk_out_buf[chunk]=0;
							chunk_out_buf_level[chunk]=0;
							to_write |= chunk_val<<c_level;
							outs[chunk].put(to_write);cnt++;
							chunk_val = chunk_val >> c_level;
							remaining-=8;
						} else {
							chunk_out_buf_level[chunk] = remaining;
							chunk_out_buf[chunk] = c | (chunk_val<<c_level);
						}
					} else {
						if(remaining<8){
							chunk_out_buf_level[chunk] = remaining;
							chunk_out_buf[chunk] = chunk_val;
						}
					}
					while(remaining>8){
						char to_write = chunk_val;
						chunk_val = chunk_val>>8;
						outs[chunk].put(to_write);cnt++;
						remaining-=8;
					}
				}
			}
		}
	} else {//use multiprecision integers, slow but handle arbitrary size :-)
		for(long block=0;block<nblocks;block++){
			boost::multiprecision::cpp_int chunk_val=0;
			boost::multiprecision::cpp_int out_buf=0;
			unsigned int output_level=0;
			for(int chunk=0;chunk<nchunks;chunk++) {
				while(output_level<chunk_width_in_bits){
					in.get(b);
					boost::multiprecision::cpp_int new_bits = 0x0FF & b;
					out_buf = out_buf | (new_bits<<output_level);
					output_level+=8;
				}
				output_level-=chunk_width_in_bits;
				chunk_val = out_buf & chunk_max_val;
				out_buf = out_buf >> chunk_width_in_bits;
				if(!out_names[chunk].empty()){//write to file
					unsigned int c_level = chunk_out_buf_level[chunk];
					unsigned int remaining = chunk_width_in_bits;
					if(c_level){
						char c = chunk_out_buf[chunk];
						remaining += c_level;
						if(remaining>=8){
							char to_write = c;
							chunk_out_buf[chunk]=0;
							chunk_out_buf_level[chunk]=0;
							boost::multiprecision::cpp_int temp = chunk_val<<c_level;
							to_write |= temp.template convert_to<char>();
							outs[chunk].put(to_write);cnt++;
							chunk_val = chunk_val >> c_level;
							remaining-=8;
						} else {
							chunk_out_buf_level[chunk] = remaining;
							boost::multiprecision::cpp_int temp = (chunk_val<<c_level);
							chunk_out_buf[chunk] = c | temp.template convert_to<unsigned int>();
						}
					} else {
						if(remaining<8){
							chunk_out_buf_level[chunk] = remaining;
							chunk_out_buf[chunk] = chunk_val.template convert_to<unsigned int>();
						}
					}
					while(remaining>8){
						char to_write = chunk_val.template convert_to<char>();
						chunk_val = chunk_val>>8;
						outs[chunk].put(to_write);cnt++;
						remaining-=8;
					}
				}
			}
		}
	}
	
	for(int chunk=0;chunk<nchunks;chunk++) {
		if(!out_names[chunk].empty()){
			unsigned int c_level = chunk_out_buf_level[chunk];
			if(c_level){
				char c = chunk_out_buf[chunk];
				outs[chunk].put(c);cnt++;
			}
			outs[chunk].close();
		}
	}
	
	return cnt;
} ERROR_MSG_CATCH("transpose_chunks(std::ifstream &in, std::ofstream &out, long total_length, long block_length, unsigned int chunk_width_in_bits)")

long split(Tcl_Interp *interp, Tcl_Channel input_channel, long start_offset, long total_length, unsigned int block_width_in_bits, unsigned int chunk_width_in_bits, std::vector<std::string> out_names) try {
	if(TCL_OK!=Tcl_SetChannelOption(interp, input_channel, "-translation", "binary")) {//use the channel in binary mode
		int errorCode = Tcl_GetErrno();
		//const char *machineReadableMessage = Tcl_ErrnoId();
		const char *hummanReadableMessage = Tcl_ErrnoMsg(errorCode);
		throw error_msg(errorCode,hummanReadableMessage);
	}
	tclchannel_istream in(input_channel);
	long cnt = split((std::ifstream&)in,start_offset,total_length, block_width_in_bits, chunk_width_in_bits,out_names);
	return cnt;
} ERROR_MSG_CATCH("raw_to_text(Tcl_Interp *interp, Tcl_Channel input_channel,	Tcl_Channel output_channel, int max_length)")

long split(std::string in_file_name, long start_offset, long total_length, unsigned int block_width_in_bits, unsigned int chunk_width_in_bits, std::vector<std::string> out_names) try {
	std::ifstream is (in_file_name, std::ifstream::binary);
	long cnt = split(is,start_offset,total_length, block_width_in_bits, chunk_width_in_bits,out_names);
	return cnt;
} ERROR_MSG_CATCH("raw_to_text(Tcl_Interp *interp, Tcl_Channel input_channel,	Tcl_Channel output_channel, int max_length)")

#define IN_CHAN_IDX 1
#define START_OFFSET_IDX 2
#define TOTAL_LENGTH_IDX 3
#define BLOCK_WIDTH_IDX 4
#define CHUNK_WIDTH_IDX 5
#define OUT_NAMES_IDX 6

 int split_cmd_core(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[], bool in_file_is_channel)  try {
	std::string in_file_name;
	long start_offset=0;
	long total_length=-1;
	int block_width_in_bits=0;
	int chunk_width_in_bits=0;
	int mode;
	in_file_name = Tcl_GetString(objv[IN_CHAN_IDX]);
	//check the file system
	Tcl_Channel input_channel;
	if(in_file_is_channel){
		input_channel = Tcl_GetChannel(interp, in_file_name.c_str(), &mode);
		if (input_channel == (Tcl_Channel) NULL) throw error_msg("input_channel is null");
		if ((mode & TCL_READABLE) == 0) {
			std::stringstream msg;
			msg << "file \""<< in_file_name<<"\" cannot be opened for reading";
			throw error_msg(msg.str());
		}
	} else {
		//input_channel = Tcl_OpenFileChannel(interp, in_file_name.c_str(), "RDONLY", 0);
		//if (input_channel == (Tcl_Channel) NULL) throw error_msg("input_channel is null");
	}
	
	if (Tcl_GetLongFromObj(interp, objv[START_OFFSET_IDX], &start_offset) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[START_OFFSET_IDX])<<"\" to long integer value";
		throw error_msg(msg.str());
	}
	if (Tcl_GetLongFromObj(interp, objv[TOTAL_LENGTH_IDX], &total_length) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[TOTAL_LENGTH_IDX])<<"\" to long integer value";
		throw error_msg(msg.str());
	}
	if (Tcl_GetIntFromObj(interp, objv[BLOCK_WIDTH_IDX], &block_width_in_bits) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[BLOCK_WIDTH_IDX])<<"\" to integer value";
		throw error_msg(msg.str());
	}
	if (Tcl_GetIntFromObj(interp, objv[CHUNK_WIDTH_IDX], &chunk_width_in_bits) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[CHUNK_WIDTH_IDX])<<"\" to integer value";
		throw error_msg(msg.str());
	}
	/*if (Tcl_GetListFromObj(interp, objv[OUT_NAMES_IDX], &out_names_list) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot convert \""<< Tcl_GetString(objv[OUT_NAMES_IDX])<<"\" to integer value";
		throw error_msg(msg.str());
	}*/
	Tcl_Obj *out_names_list = objv[OUT_NAMES_IDX];
	Tcl_Obj **listObjv;
	int list_length;
	if (Tcl_ListObjGetElements (interp, out_names_list, &list_length, &listObjv) != TCL_OK) {
        std::stringstream msg;
		msg << "cannot get elements from list \""<< Tcl_GetString(objv[OUT_NAMES_IDX])<<"\"";
		throw error_msg(msg.str());
    }
	/*if (Tcl_ListObjLength(interp,out_names_list,&list_length) != TCL_OK) {
		std::stringstream msg;
		msg << "cannot get the length of \""<< Tcl_GetString(objv[OUT_NAMES_IDX])<<"\"";
		throw error_msg(msg.str());
	}*/
	std::vector<std::string> out_names(list_length);
	for(unsigned int i=0;i<list_length;i++){
		out_names[i] = Tcl_GetString(listObjv[i]);
	}
	long cnt;
	if(in_file_is_channel){
		cnt=split(interp,input_channel,start_offset,total_length,(unsigned int)block_width_in_bits,(unsigned int)chunk_width_in_bits,out_names);
	} else {
		cnt=split(in_file_name,start_offset,total_length,(unsigned int)block_width_in_bits,(unsigned int)chunk_width_in_bits,out_names);
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

int split_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	return split_cmd_core(cdata,interp,objc,objv,false);//input file passed by file name
}
/*
 //our channlestream don't support seekg correctly
int split_chan_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	return split_cmd_core(cdata,interp,objc,objv,true);//input file passed by channel
}
*/