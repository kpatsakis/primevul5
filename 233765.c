print_line_tail (const char *beg, const char *lim, const char *line_color)
{
  size_t eol_size;
  size_t tail_size;

  eol_size   = (lim > beg && lim[-1] == eolbyte);
  eol_size  += (lim - eol_size > beg && lim[-(1 + eol_size)] == '\r');
  tail_size  =  lim - eol_size - beg;

  if (tail_size > 0)
    {
      pr_sgr_start (line_color);
      fwrite (beg, 1, tail_size, stdout);
      beg += tail_size;
      pr_sgr_end (line_color);
    }

  return beg;
}