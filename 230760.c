lib_do_one_event_core(argc, argv, self, is_ip)
    int   argc;
    VALUE *argv;
    VALUE self;
    int   is_ip;
{
    volatile VALUE vflags;
    int flags;
    int found_event;

    if (!NIL_P(eventloop_thread)) {
        rb_raise(rb_eRuntimeError, "eventloop is already running");
    }

    tcl_stubs_check();

    if (rb_scan_args(argc, argv, "01", &vflags) == 0) {
        flags = TCL_ALL_EVENTS | TCL_DONT_WAIT;
    } else {
        Check_Type(vflags, T_FIXNUM);
        flags = FIX2INT(vflags);
    }

    if (rb_safe_level() >=1 && OBJ_TAINTED(vflags)) {
      flags |= TCL_DONT_WAIT;
    }

    if (is_ip) {
        /* check IP */
        struct tcltkip *ptr = get_ip(self);

        /* ip is deleted? */
        if (deleted_ip(ptr)) {
            return Qfalse;
        }

        if (Tcl_GetMaster(ptr->ip) != (Tcl_Interp*)NULL) {
            /* slave IP */
            flags |= TCL_DONT_WAIT;
        }
    }

    /* found_event = Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT); */
    found_event = Tcl_DoOneEvent(flags);

    if (pending_exception_check0()) {
        return Qfalse;
    }

    if (found_event) {
        return Qtrue;
    } else {
        return Qfalse;
    }
}