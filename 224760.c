unbind_variable (name)
     const char *name;
{
  return makunbound (name, shell_variables);
}