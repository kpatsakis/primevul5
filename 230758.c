tcltklib_compile_info(void)
{
    volatile VALUE ret;
    size_t size;
    static CONST char form[]
      = "tcltklib %s :: Ruby%s (%s) %s pthread :: Tcl%s(%s)/Tk%s(%s) %s";
    char *info;

    size = strlen(form)
        + strlen(TCLTKLIB_RELEASE_DATE)
        + strlen(RUBY_VERSION)
        + strlen(RUBY_RELEASE_DATE)
        + strlen("without")
        + strlen(TCL_PATCH_LEVEL)
        + strlen("without stub")
        + strlen(TK_PATCH_LEVEL)
        + strlen("without stub")
        + strlen("unknown tcl_threads");

    info = ALLOC_N(char, size);
    /* info = ckalloc(sizeof(char) * size); */ /* SEGV */

    sprintf(info, form,
            TCLTKLIB_RELEASE_DATE,
            RUBY_VERSION, RUBY_RELEASE_DATE,
#ifdef HAVE_NATIVETHREAD
            "with",
#else
            "without",
#endif
            TCL_PATCH_LEVEL,
#ifdef USE_TCL_STUBS
            "with stub",
#else
            "without stub",
#endif
            TK_PATCH_LEVEL,
#ifdef USE_TK_STUBS
            "with stub",
#else
            "without stub",
#endif
#ifdef WITH_TCL_ENABLE_THREAD
# if WITH_TCL_ENABLE_THREAD
            "with tcl_threads"
# else
            "without tcl_threads"
# endif
#else
            "unknown tcl_threads"
#endif
        );

    ret = rb_obj_freeze(rb_str_new2(info));

    xfree(info);
    /* ckfree(info); */

    return ret;
}