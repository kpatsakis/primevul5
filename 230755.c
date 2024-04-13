ip_ruby_cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char *argv[];
#endif
{
    volatile VALUE receiver;
    volatile ID method;
    volatile VALUE args;
    char *str;
    int i;
    int  len;
    struct cmd_body_arg *arg;
    int thr_crit_bup;
    VALUE old_gc;
    int code;

    if (interp == (Tcl_Interp*)NULL) {
        rbtk_pending_exception = rb_exc_new2(rb_eRuntimeError,
                                             "IP is deleted");
        return TCL_ERROR;
    }

    if (argc < 3) {
#if 0
        rb_raise(rb_eArgError, "too few arguments");
#else
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, "too few arguments", (char *)NULL);
        rbtk_pending_exception = rb_exc_new2(rb_eArgError,
                                             Tcl_GetStringResult(interp));
        return TCL_ERROR;
#endif
    }

    /* get arguments from Tcl objects */
    thr_crit_bup = rb_thread_critical;
    rb_thread_critical = Qtrue;
    old_gc = rb_gc_disable();

    /* get receiver */
#if TCL_MAJOR_VERSION >= 8
    str = Tcl_GetStringFromObj(argv[1], &len);
#else /* TCL_MAJOR_VERSION < 8 */
    str = argv[1];
#endif
    DUMP2("receiver:%s",str);
    /* receiver = rb_protect(ip_ruby_cmd_receiver_get, (VALUE)str, &code); */
    receiver = ip_ruby_cmd_receiver_get(str);
    if (NIL_P(receiver)) {
#if 0
        rb_raise(rb_eArgError,
                 "unknown class/module/global-variable '%s'", str);
#else
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, "unknown class/module/global-variable '",
                         str, "'", (char *)NULL);
        rbtk_pending_exception = rb_exc_new2(rb_eArgError,
                                             Tcl_GetStringResult(interp));
        if (old_gc == Qfalse) rb_gc_enable();
        return TCL_ERROR;
#endif
    }

    /* get metrhod */
#if TCL_MAJOR_VERSION >= 8
    str = Tcl_GetStringFromObj(argv[2], &len);
#else /* TCL_MAJOR_VERSION < 8 */
    str = argv[2];
#endif
    method = rb_intern(str);

    /* get args */
    args = rb_ary_new2(argc - 2);
    for(i = 3; i < argc; i++) {
        VALUE s;
#if TCL_MAJOR_VERSION >= 8
        str = Tcl_GetStringFromObj(argv[i], &len);
        s = rb_tainted_str_new(str, len);
#else /* TCL_MAJOR_VERSION < 8 */
        str = argv[i];
        s = rb_tainted_str_new2(str);
#endif
        DUMP2("arg:%s",str);
#ifndef HAVE_STRUCT_RARRAY_LEN
        rb_ary_push(args, s);
#else
        RARRAY(args)->ptr[RARRAY(args)->len++] = s;
#endif
    }

    if (old_gc == Qfalse) rb_gc_enable();
    rb_thread_critical = thr_crit_bup;

    /* allocate */
    arg = ALLOC(struct cmd_body_arg);
    /* arg = RbTk_ALLOC_N(struct cmd_body_arg, 1); */

    arg->receiver = receiver;
    arg->method = method;
    arg->args = args;

    /* evaluate the argument string by ruby */
    code = tcl_protect(interp, ip_ruby_cmd_core, (VALUE)arg);

    xfree(arg);
    /* ckfree((char*)arg); */

    return code;
}