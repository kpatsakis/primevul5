restore_pipestatus_array (a)
     ARRAY *a;
{
  SHELL_VAR *v;
  ARRAY *a2;

  v = find_variable ("PIPESTATUS");
  /* XXX - should we still assign even if existing value is NULL? */
  if (v == 0 || array_p (v) == 0 || array_cell (v) == 0)
    return;

  a2 = array_cell (v);
  var_setarray (v, a); 

  array_dispose (a2);
}