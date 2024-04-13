make_funcname_visible (on_or_off)
     int on_or_off;
{
  SHELL_VAR *v;

  v = find_variable ("FUNCNAME");
  if (v == 0 || v->dynamic_value == 0)
    return;

  if (on_or_off)
    VUNSETATTR (v, att_invisible);
  else
    VSETATTR (v, att_invisible);
}