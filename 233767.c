reset (int fd, char const *file, struct stats *stats)
{
  if (! pagesize)
    {
      pagesize = getpagesize ();
      if (pagesize == 0 || 2 * pagesize + 1 <= pagesize)
        abort ();
      bufalloc = ALIGN_TO (INITIAL_BUFSIZE, pagesize) + pagesize + 1;
      buffer = xmalloc (bufalloc);
    }

  bufbeg = buflim = ALIGN_TO (buffer + 1, pagesize);
  bufbeg[-1] = eolbyte;
  bufdesc = fd;

  if (S_ISREG (stats->stat.st_mode))
    {
      if (file)
        bufoffset = 0;
      else
        {
          bufoffset = lseek (fd, 0, SEEK_CUR);
          if (bufoffset < 0)
            {
              suppressible_error (_("lseek failed"), errno);
              return 0;
            }
        }
    }
  return 1;
}