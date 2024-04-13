coproc_setvars (cp)
     struct coproc *cp;
{
  SHELL_VAR *v;
  char *namevar, *t;
  int l;
#if defined (ARRAY_VARS)
  arrayind_t ind;
#endif

  if (cp->c_name == 0)
    return;

  l = strlen (cp->c_name);
  namevar = xmalloc (l + 16);

#if defined (ARRAY_VARS)
  v = find_variable (cp->c_name);
  if (v == 0)
    v = make_new_array_variable (cp->c_name);
  if (array_p (v) == 0)
    v = convert_var_to_array (v);

  t = itos (cp->c_rfd);
  ind = 0;
  v = bind_array_variable (cp->c_name, ind, t, 0);
  free (t);

  t = itos (cp->c_wfd);
  ind = 1;
  bind_array_variable (cp->c_name, ind, t, 0);
  free (t);
#else
  sprintf (namevar, "%s_READ", cp->c_name);
  t = itos (cp->c_rfd);
  bind_variable (namevar, t, 0);
  free (t);
  sprintf (namevar, "%s_WRITE", cp->c_name);
  t = itos (cp->c_wfd);
  bind_variable (namevar, t, 0);
  free (t);
#endif

  sprintf (namevar, "%s_PID", cp->c_name);
  t = itos (cp->c_pid);
  bind_variable (namevar, t, 0);
  free (t);

  free (namevar);
}