get_funcname (self)
     SHELL_VAR *self;
{
#if ! defined (ARRAY_VARS)
  char *t;
  if (variable_context && this_shell_function)
    {
      FREE (value_cell (self));
      t = savestring (this_shell_function->name);
      var_setvalue (self, t);
    }
#endif
  return (self);
}