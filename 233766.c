prline (char const *beg, char const *lim, int sep)
{
  int matching;
  const char *line_color;
  const char *match_color;

  if (!only_matching)
    print_line_head (beg, lim, sep);

  matching = (sep == SEP_CHAR_SELECTED) ^ !!out_invert;

  if (color_option)
    {
      line_color = (((sep == SEP_CHAR_SELECTED)
                     ^ (out_invert && (color_option < 0)))
                    ? selected_line_color  : context_line_color);
      match_color = (sep == SEP_CHAR_SELECTED
                     ? selected_match_color : context_match_color);
    }
  else
    line_color = match_color = NULL; /* Shouldn't be used.  */

  if ((only_matching && matching)
      || (color_option  && (*line_color || *match_color)))
    {
      /* We already know that non-matching lines have no match (to colorize).  */
      if (matching && (only_matching || *match_color))
        beg = print_line_middle (beg, lim, line_color, match_color);

      /* FIXME: this test may be removable.  */
      if (!only_matching && *line_color)
        beg = print_line_tail (beg, lim, line_color);
    }

  if (!only_matching && lim > beg)
    fwrite (beg, 1, lim - beg, stdout);

  if (ferror (stdout))
    {
      write_error_seen = 1;
      error (EXIT_TROUBLE, 0, _("write error"));
    }

  lastout = lim;

  if (line_buffered)
    fflush (stdout);
}