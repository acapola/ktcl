 #include <tcl.h>

#define IN_CHAN_IDX 0
#define OUT_CHAN_IDX 1
#define MAX_LENGTH_IDX 2

 static int raw_to_text_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	Tcl_Channel input_channel;
	Tcl_Channel output_channel;
	int max_length;
	int mode;
	
	if(objc!=3) goto wrongArgs;
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
	
	if (Tcl_GetIntFromObj(interp, objv[MAX_LENGTH_IDX], &max_length) != TCL_OK) {
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, Tcl_NewStringObj("Hello, World!", -1));
	return TCL_OK;

wrongArgs:
    Tcl_AppendResult (interp, "wrong # args: should be:\n",
	    "  ",
	    Tcl_GetString(objv[0]),
	    " file_in file_out ?max_length?",
	    (char *) NULL);
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
 