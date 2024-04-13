fillbuf (size_t save, struct stats const *stats)
{
  size_t fillsize = 0;
  int cc = 1;
  char *readbuf;
  size_t readsize;

  /* Offset from start of buffer to start of old stuff
     that we want to save.  */
  size_t saved_offset = buflim - save - buffer;

  if (pagesize <= buffer + bufalloc - buflim)
    {
      readbuf = buflim;
      bufbeg = buflim - save;
    }
  else
    {
      size_t minsize = save + pagesize;
      size_t newsize;
      size_t newalloc;
      char *newbuf;

      /* Grow newsize until it is at least as great as minsize.  */
      for (newsize = bufalloc - pagesize - 1; newsize < minsize; newsize *= 2)
        if (newsize * 2 < newsize || newsize * 2 + pagesize + 1 < newsize * 2)
          xalloc_die ();

      /* Try not to allocate more memory than the file size indicates,
         as that might cause unnecessary memory exhaustion if the file
         is large.  However, do not use the original file size as a
         heuristic if we've already read past the file end, as most
         likely the file is growing.  */
      if (S_ISREG (stats->stat.st_mode))
        {
          off_t to_be_read = stats->stat.st_size - bufoffset;
          off_t maxsize_off = save + to_be_read;
          if (0 <= to_be_read && to_be_read <= maxsize_off
              && maxsize_off == (size_t) maxsize_off
              && minsize <= (size_t) maxsize_off
              && (size_t) maxsize_off < newsize)
            newsize = maxsize_off;
        }

      /* Add enough room so that the buffer is aligned and has room
         for byte sentinels fore and aft.  */
      newalloc = newsize + pagesize + 1;

      newbuf = bufalloc < newalloc ? xmalloc (bufalloc = newalloc) : buffer;
      readbuf = ALIGN_TO (newbuf + 1 + save, pagesize);
      bufbeg = readbuf - save;
      memmove (bufbeg, buffer + saved_offset, save);
      bufbeg[-1] = eolbyte;
      if (newbuf != buffer)
        {
          free (buffer);
          buffer = newbuf;
        }
    }

  readsize = buffer + bufalloc - readbuf;
  readsize -= readsize % pagesize;

  if (! fillsize)
    {
      ssize_t bytesread;
      while ((bytesread = read (bufdesc, readbuf, readsize)) < 0
             && errno == EINTR)
        continue;
      if (bytesread < 0)
        cc = 0;
      else
        fillsize = bytesread;
    }

  bufoffset += fillsize;
#if defined HAVE_DOS_FILE_CONTENTS
  if (fillsize)
    fillsize = undossify_input (readbuf, fillsize);
#endif
  buflim = readbuf + fillsize;
  return cc;
}