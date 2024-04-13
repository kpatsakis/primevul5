select_query (list, list_len, prompt, print_menu)
     WORD_LIST *list;
     int list_len;
     char *prompt;
     int print_menu;
{
  int max_elem_len, indices_len, len;
  intmax_t reply;
  WORD_LIST *l;
  char *repl_string, *t;

#if 0
  t = get_string_value ("LINES");
  LINES = (t && *t) ? atoi (t) : 24;
#endif
  t = get_string_value ("COLUMNS");
  COLS =  (t && *t) ? atoi (t) : 80;

#if 0
  t = get_string_value ("TABSIZE");
  tabsize = (t && *t) ? atoi (t) : 8;
  if (tabsize <= 0)
    tabsize = 8;
#else
  tabsize = 8;
#endif

  max_elem_len = 0;
  for (l = list; l; l = l->next)
    {
      len = displen (l->word->word);
      if (len > max_elem_len)
	max_elem_len = len;
    }
  indices_len = NUMBER_LEN (list_len);
  max_elem_len += indices_len + RP_SPACE_LEN + 2;

  while (1)
    {
      if (print_menu)
	print_select_list (list, list_len, max_elem_len, indices_len);
      fprintf (stderr, "%s", prompt);
      fflush (stderr);
      QUIT;

      if (read_builtin ((WORD_LIST *)NULL) != EXECUTION_SUCCESS)
	{
	  putchar ('\n');
	  return ((char *)NULL);
	}
      repl_string = get_string_value ("REPLY");
      if (*repl_string == 0)
	{
	  print_menu = 1;
	  continue;
	}
      if (legal_number (repl_string, &reply) == 0)
	return "";
      if (reply < 1 || reply > list_len)
	return "";

      for (l = list; l && --reply; l = l->next)
	;
      return (l->word->word);		/* XXX - can't be null? */
    }
}