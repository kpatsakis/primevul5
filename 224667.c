save_pipestatus_array ()
{
  SHELL_VAR *v;
  ARRAY *a, *a2;

  v = find_variable ("PIPESTATUS");
  if (v == 0 || array_p (v) == 0 || array_cell (v) == 0)
    return ((ARRAY *)NULL);
    
  a = array_cell (v);
  a2 = array_copy (array_cell (v));

  return a2;
}