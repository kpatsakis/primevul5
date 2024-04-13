error_prolog (print_lineno)
     int print_lineno;
{
  char *ename;
  int line;

  ename = get_name_for_error ();
  line = (print_lineno && interactive_shell == 0) ? executing_line_number () : -1;

  if (line > 0)
    fprintf (stderr, "%s:%s%d: ", ename, gnu_error_format ? "" : _(" line "), line);
  else
    fprintf (stderr, "%s: ", ename);
}