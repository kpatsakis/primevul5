sv_histsize (name)
     char *name;
{
  char *temp;
  intmax_t num;
  int hmax;

  temp = get_string_value (name);

  if (temp && *temp)
    {
      if (legal_number (temp, &num))
	{
	  hmax = num;
	  if (hmax < 0 && name[4] == 'S')
	    unstifle_history ();	/* unstifle history if HISTSIZE < 0 */
	  else if (name[4] == 'S')
	    {
	      stifle_history (hmax);
	      hmax = where_history ();
	      if (history_lines_this_session > hmax)
		history_lines_this_session = hmax;
	    }
	  else if (hmax >= 0)	/* truncate HISTFILE if HISTFILESIZE >= 0 */
	    {
	      history_truncate_file (get_string_value ("HISTFILE"), hmax);
	      if (hmax <= history_lines_in_file)
		history_lines_in_file = hmax;
	    }
	}
    }
  else if (name[4] == 'S')
    unstifle_history ();
}