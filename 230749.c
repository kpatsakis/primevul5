setup_rubytkkit(void)
{
  init_static_tcltk_packages();

  {
    ID const_id;
    const_id = rb_intern(RUBYTK_KITPATH_CONST_NAME);

    if (rb_const_defined(rb_cObject, const_id)) {
      volatile VALUE pathobj;
      pathobj = rb_const_get(rb_cObject, const_id);

      if (rb_obj_is_kind_of(pathobj, rb_cString)) {
#ifdef HAVE_RUBY_ENCODING_H
	pathobj = rb_str_export_to_enc(pathobj, rb_utf8_encoding());
#endif
	set_rubytk_kitpath(RSTRING_PTR(pathobj));
      }
    }
  }

#ifdef CREATE_RUBYTK_KIT
  if (rubytk_kitpath == NULL) {
#ifdef __WIN32__
    /* rbtk_win32_SetHINSTANCE("tcltklib.so"); */
    {
      volatile VALUE basename;
      basename = rb_funcall(rb_cFile, rb_intern("basename"), 1,
			    rb_str_new2(rb_sourcefile()));
      rbtk_win32_SetHINSTANCE(RSTRING_PTR(basename));
    }
#endif
    set_rubytk_kitpath(rb_sourcefile());
  }
#endif

  if (rubytk_kitpath == NULL) {
    set_rubytk_kitpath(Tcl_GetNameOfExecutable());
  }

  TclSetPreInitScript(rubytkkit_preInitCmd);
}