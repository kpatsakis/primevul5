print_line_head (char const *beg, char const *lim, int sep)
{
  int pending_sep = 0;

  if (out_file)
    {
      print_filename ();
      if (filename_mask)
        pending_sep = 1;
      else
        fputc (0, stdout);
    }

  if (out_line)
    {
      if (lastnl < lim)
        {
          nlscan (beg);
          totalnl = add_count (totalnl, 1);
          lastnl = lim;
        }
      if (pending_sep)
        print_sep (sep);
      print_offset (totalnl, 4, line_num_color);
      pending_sep = 1;
    }

  if (out_byte)
    {
      uintmax_t pos = add_count (totalcc, beg - bufbeg);
#if defined HAVE_DOS_FILE_CONTENTS
      pos = dossified_pos (pos);
#endif
      if (pending_sep)
        print_sep (sep);
      print_offset (pos, 6, byte_num_color);
      pending_sep = 1;
    }

  if (pending_sep)
    {
      /* This assumes sep is one column wide.
         Try doing this any other way with Unicode
         (and its combining and wide characters)
         filenames and you're wasting your efforts.  */
      if (align_tabs)
        fputs ("\t\b", stdout);

      print_sep (sep);
    }
}