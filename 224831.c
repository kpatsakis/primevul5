find_function (name)
     const char *name;
{
  return (hash_lookup (name, shell_functions));
}