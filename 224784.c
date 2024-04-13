sh_set_lines_and_columns (lines, cols)
     int lines, cols;
{
  char val[INT_STRLEN_BOUND(int) + 1], *v;

#if defined (READLINE)
  /* If we are currently assigning to LINES or COLUMNS, don't do anything. */
  if (winsize_assignment)
    return;
#endif

  v = inttostr (lines, val, sizeof (val));
  bind_variable ("LINES", v, 0);

  v = inttostr (cols, val, sizeof (val));
  bind_variable ("COLUMNS", v, 0);
}