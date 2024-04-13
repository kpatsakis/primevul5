init_dynamic_array_var (name, getfunc, setfunc, attrs)
     char *name;
     sh_var_value_func_t *getfunc;
     sh_var_assign_func_t *setfunc;
     int attrs;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v)
    return (v);
  INIT_DYNAMIC_ARRAY_VAR (name, getfunc, setfunc);
  if (attrs)
    VSETATTR (v, attrs);
  return v;
}