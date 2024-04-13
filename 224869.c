coproc_unsetvars (cp)
     struct coproc *cp;
{
  int l;
  char *namevar;

  if (cp->c_name == 0)
    return;

  l = strlen (cp->c_name);
  namevar = xmalloc (l + 16);

  sprintf (namevar, "%s_PID", cp->c_name);
  unbind_variable (namevar);  

#if defined (ARRAY_VARS)
  unbind_variable (cp->c_name);
#else
  sprintf (namevar, "%s_READ", cp->c_name);
  unbind_variable (namevar);
  sprintf (namevar, "%s_WRITE", cp->c_name);
  unbind_variable (namevar);
#endif  

  free (namevar);
}