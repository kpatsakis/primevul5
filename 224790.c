init_funcname_var ()
{
  SHELL_VAR *v;

  v = find_variable ("FUNCNAME");
  if (v)
    return v;
#if defined (ARRAY_VARS)
  INIT_DYNAMIC_ARRAY_VAR ("FUNCNAME", get_funcname, null_array_assign);
#else
  INIT_DYNAMIC_VAR ("FUNCNAME", (char *)NULL, get_funcname, null_assign);
#endif
  VSETATTR (v, att_invisible|att_noassign);
  return v;
}