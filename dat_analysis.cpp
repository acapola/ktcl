#include <tcl.h>

int raw_to_text_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int transpose_cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int /*DLLEXPORT*/ dat_analysis_init(Tcl_Interp *interp) {
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
        Tcl_CreateObjCommand(interp, "dat_analysis::transpose", transpose_cmd, NULL, NULL);
        Tcl_PkgProvide(interp, "dat_analysis", "1.0");
        return TCL_OK;
 }
 
 