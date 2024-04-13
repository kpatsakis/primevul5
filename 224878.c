export_environment_candidate (var)
     SHELL_VAR *var;
{
  return (exported_p (var) && (invisible_p (var) == 0 || imported_p (var)));
}