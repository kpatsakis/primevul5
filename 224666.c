push_context (name, is_subshell, tempvars)
     char *name;	/* function name */
     int is_subshell;
     HASH_TABLE *tempvars;
{
  if (is_subshell == 0)
    push_dollar_vars ();
  variable_context++;
  push_var_context (name, VC_FUNCENV, tempvars);
}