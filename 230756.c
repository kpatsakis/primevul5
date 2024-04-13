ip_RubyExitCommand(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char *argv[];
#endif
#endif
{
    int state;
    char *cmd, *param;
#if TCL_MAJOR_VERSION < 8
    char *endptr;
    cmd = argv[0];
#endif

    DUMP1("start ip_RubyExitCommand");

#if TCL_MAJOR_VERSION >= 8
    /* cmd = Tcl_GetString(argv[0]); */
    cmd = Tcl_GetStringFromObj(argv[0], (int*)NULL);
#endif

    if (argc < 1 || argc > 2) {
        /* argument error */
        Tcl_AppendResult(interp,
                         "wrong number of arguments: should be \"",
                         cmd, " ?returnCode?\"", (char *)NULL);
        return TCL_ERROR;
    }

    if (interp == (Tcl_Interp*)NULL) return TCL_OK;

    Tcl_ResetResult(interp);

    if (Tcl_IsSafe(interp)) {
	if (!Tcl_InterpDeleted(interp)) {
	  ip_finalize(interp);

	  Tcl_DeleteInterp(interp);
	  Tcl_Release(interp);
	}
        return TCL_OK;
    }

    switch(argc) {
    case 1:
        /* rb_exit(0); */ /* not return if succeed */
        Tcl_AppendResult(interp,
                         "fail to call \"", cmd, "\"", (char *)NULL);

        rbtk_pending_exception = rb_exc_new2(rb_eSystemExit,
                                             Tcl_GetStringResult(interp));
        rb_iv_set(rbtk_pending_exception, "status", INT2FIX(0));

        return TCL_RETURN;

    case 2:
#if TCL_MAJOR_VERSION >= 8
        if (Tcl_GetIntFromObj(interp, argv[1], &state) == TCL_ERROR) {
            return TCL_ERROR;
        }
        /* param = Tcl_GetString(argv[1]); */
        param = Tcl_GetStringFromObj(argv[1], (int*)NULL);
#else /* TCL_MAJOR_VERSION < 8 */
        state = (int)strtol(argv[1], &endptr, 0);
        if (*endptr) {
            Tcl_AppendResult(interp,
                             "expected integer but got \"",
                             argv[1], "\"", (char *)NULL);
            return TCL_ERROR;
        }
        param = argv[1];
#endif
        /* rb_exit(state); */ /* not return if succeed */

        Tcl_AppendResult(interp, "fail to call \"", cmd, " ",
                         param, "\"", (char *)NULL);

        rbtk_pending_exception = rb_exc_new2(rb_eSystemExit,
                                             Tcl_GetStringResult(interp));
        rb_iv_set(rbtk_pending_exception, "status", INT2FIX(state));

        return TCL_RETURN;

    default:
        /* arguemnt error */
        Tcl_AppendResult(interp,
                         "wrong number of arguments: should be \"",
                         cmd, " ?returnCode?\"", (char *)NULL);
        return TCL_ERROR;
    }
}