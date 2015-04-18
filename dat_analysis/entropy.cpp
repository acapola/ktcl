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

long entropy(std::ifstream &in, long total_length, unsigned int block_width_in_bits, unsigned int chunk_width_in_bits,
		double *min_entropy, 
		double *shannon_entropy,
		unsigned int *min_value,
		unsigned int *max_value
	) try {
	long cnt=0;
	char b;
	unsigned int nchunks = block_width_in_bits / chunk_width_in_bits;
	unsigned int nsymbols = 1<<chunk_width_in_bits;
	// Create a 2D array that is nchunks x nsymbols
	typedef boost::multi_array<unsigned int, 2> array2d_unint_t;
	typedef array2d_unint_t::index array2d_unint_index_t;
	array2d_unint_t symbols_cnt(boost::extents[nchunks][nsymbols]);
	for(array2d_unint_index_t chunk_index = 0;chunk_index<nchunks;chunk_index++){
		for(array2d_unint_index_t symbol_index = 0; symbol_index < nsymbols; symbol_index++){
			symbols_cnt[chunk_index][symbol_index]=0;
		}
	}
	for(int chunk=0;chunk<nchunks;chunk++){
		min_value[chunk]=(1L<<chunk_width_in_bits)-1;
		max_value[chunk]=0;
	}
	long nblocks = (total_length*8) / block_width_in_bits;
	for(long block=0;block<nblocks;block++){
		for(int chunk=0;chunk<nchunks;chunk++) {
			in.get(b);
			unsigned int chunk_val = 0x0FF & b;
			if(chunk_val<min_value[chunk]) min_value[chunk] = chunk_val;
			if(chunk_val>max_value[chunk]) max_value[chunk] = chunk_val;
			array2d_unint_index_t chunk_index = chunk;
			array2d_unint_index_t symbol_index = chunk_val;
			symbols_cnt[chunk_index][symbol_index]=symbols_cnt[chunk_index][symbol_index]+1;
			cnt++;
		}
	}
	
	for(int chunk=0;chunk<nchunks;chunk++) {
		array2d_unint_index_t chunk_index = chunk;
		unsigned int max = 0;
		double sumpx=0;
		for(array2d_unint_index_t symbol_index = 0; symbol_index < nsymbols; symbol_index++){
			unsigned int cnt = symbols_cnt[chunk_index][symbol_index];
			if(cnt>max) max = cnt;
			double p = ((double)cnt)/nblocks;
			if(p!=0) sumpx += p*log2(p);
		}
		double pmax = ((double)max)/nblocks;
		min_entropy[chunk] = -log2(pmax);
		shannon_entropy[chunk] = -sumpx;
	}
	
	return cnt;
} ERROR_MSG_CATCH("transpose_chunks(std::ifstream &in, std::ofstream &out, long total_length, long block_length, unsigned int chunk_width_in_bits)")

long entropy(Tcl_Interp *interp, Tcl_Channel input_channel,	long total_length, unsigned int block_length_in_bits, unsigned int chunk_width_in_bits,
		double *min_entropy, 
		double *shannon_entropy,
		unsigned int *min_value,
		unsigned int *max_value
	) try {
	if(TCL_OK!=Tcl_SetChannelOption(interp, input_channel, "-translation", "binary")) {//use the channel in binary mode
		int errorCode = Tcl_GetErrno();
		//const char *machineReadableMessage = Tcl_ErrnoId();
		const char *hummanReadableMessage = Tcl_ErrnoMsg(errorCode);
		throw error_msg(errorCode,hummanReadableMessage);
	}
	tclchannel_istream in(input_channel);
	long cnt = entropy((std::ifstream&)in,total_length, block_length_in_bits, chunk_width_in_bits, 
		min_entropy, shannon_entropy,min_value,max_value);
	return cnt;
} ERROR_MSG_CATCH("raw_to_text(Tcl_Interp *interp, Tcl_Channel input_channel,	Tcl_Channel output_channel, int max_length)")

#define IN_CHAN_IDX 1
#define TOTAL_LENGTH_IDX 2
#define BLOCK_WIDTH_IDX 3
#define CHUNK_WIDTH_IDX 4

 int entropy_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])  try {
	std::string in_file_name;
	std::string out_file_name;
	long total_length=-1;
	int block_width_in_bits=0;
	int chunk_width_in_bits=0;
	int mode;
	if((objc<4)&&(objc>5)) {
		throw error_msg("wrong # args: should be:\n   file_in total_length block_width_in_bits [chunk_width_in_bits]");
	}
	in_file_name = Tcl_GetString(objv[IN_CHAN_IDX]);
	//check the file system
	Tcl_Channel input_channel = Tcl_GetChannel(interp, in_file_name.c_str(), &mode);
	if (input_channel == (Tcl_Channel) NULL) throw error_msg("input_channel is null");
	if ((mode & TCL_READABLE) == 0) {
		std::stringstream msg;
		msg << "file \""<< in_file_name<<"\" cannot be opened for reading";
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
	if (objc>CHUNK_WIDTH_IDX){
		if (Tcl_GetIntFromObj(interp, objv[CHUNK_WIDTH_IDX], &chunk_width_in_bits) != TCL_OK) {
			std::stringstream msg;
			msg << "cannot convert \""<< Tcl_GetString(objv[CHUNK_WIDTH_IDX])<<"\" to integer value";
			throw error_msg(msg.str());
		}
	} else {
		chunk_width_in_bits = block_width_in_bits;
	}
	int nchunks = block_width_in_bits/chunk_width_in_bits;
	double *min_entropy = new double[nchunks];
	double *shannon_entropy = new double[nchunks];
	unsigned int *min_value = new unsigned int[nchunks];
	unsigned int *max_value = new unsigned int[nchunks];
	//for(int i=0;i<nchunks;i++) entropy[i] = i;
	entropy(interp,input_channel,total_length,(unsigned int)block_width_in_bits,(unsigned int)chunk_width_in_bits,
			min_entropy, shannon_entropy,min_value,max_value);
	//printf("nchunks=%d\n",nchunks);
	Tcl_Obj *min_entropy_list=Tcl_NewListObj(0, 0);
	Tcl_Obj *shannon_entropy_list=Tcl_NewListObj(0, 0);
	Tcl_Obj *min_value_list=Tcl_NewListObj(0, 0);
	Tcl_Obj *max_value_list=Tcl_NewListObj(0, 0);
	Tcl_Obj *keyPtr;
	Tcl_Obj *valuePtr;
	for(int i=0;i<nchunks;i++) {
		valuePtr = Tcl_NewDoubleObj(min_entropy[i]);
		if (Tcl_ListObjAppendElement(interp, min_entropy_list, valuePtr) != TCL_OK){
			std::stringstream msg;
			msg << "Tcl_ListObjAppendElement error in min_entropy_list";
			throw error_msg(msg.str());
		}
		valuePtr = Tcl_NewDoubleObj(shannon_entropy[i]);
		if (Tcl_ListObjAppendElement(interp, shannon_entropy_list, valuePtr) != TCL_OK){
			std::stringstream msg;
			msg << "Tcl_ListObjAppendElement error in shannon_entropy_list";
			throw error_msg(msg.str());
		}
		valuePtr = Tcl_NewLongObj(min_value[i]);
		if (Tcl_ListObjAppendElement(interp, min_value_list, valuePtr) != TCL_OK){
			std::stringstream msg;
			msg << "Tcl_ListObjAppendElement error in min_value_list";
			throw error_msg(msg.str());
		}
		valuePtr = Tcl_NewLongObj(max_value[i]);
		if (Tcl_ListObjAppendElement(interp, max_value_list, valuePtr) != TCL_OK){
			std::stringstream msg;
			msg << "Tcl_ListObjAppendElement error in max_value_list";
			throw error_msg(msg.str());
		}
	}
	delete min_entropy;
	delete shannon_entropy;
	delete min_value;
	delete max_value;
	std::string str = "min_entropy";
	keyPtr = Tcl_NewStringObj(str.c_str(), str.length());
	valuePtr=min_entropy_list;
	Tcl_Obj *result=Tcl_NewDictObj();
	if (Tcl_DictObjPut(interp, result, keyPtr, valuePtr) != TCL_OK){
		std::stringstream msg;
		msg << "Tcl_DictObjPut error (result,min_entropy_list)";
		throw error_msg(msg.str());
	}
	str = "shannon_entropy";
	keyPtr = Tcl_NewStringObj(str.c_str(), str.length());
	valuePtr=shannon_entropy_list;
	if (Tcl_DictObjPut(interp, result, keyPtr, valuePtr) != TCL_OK){
		std::stringstream msg;
		msg << "Tcl_DictObjPut error (result,shannon_entropy_list)";
		throw error_msg(msg.str());
	}
	str = "min_value";
	keyPtr = Tcl_NewStringObj(str.c_str(), str.length());
	valuePtr=min_value_list;
	if (Tcl_DictObjPut(interp, result, keyPtr, valuePtr) != TCL_OK){
		std::stringstream msg;
		msg << "Tcl_DictObjPut error (result,min_value_list)";
		throw error_msg(msg.str());
	}
	str = "max_value";
	keyPtr = Tcl_NewStringObj(str.c_str(), str.length());
	valuePtr=max_value_list;
	if (Tcl_DictObjPut(interp, result, keyPtr, valuePtr) != TCL_OK){
		std::stringstream msg;
		msg << "Tcl_DictObjPut error (result,max_value_list)";
		throw error_msg(msg.str());
	}
	
	Tcl_SetObjResult(interp, result);
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
