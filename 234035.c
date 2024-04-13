sparse_write (int fildes, char *buf, size_t nbytes, bool flush)
{
  size_t nwritten = 0;
  ssize_t n;
  char *start_ptr = buf;

  static off_t delayed_seek_count = 0;
  off_t seek_count = 0;

  enum { begin, in_zeros, not_in_zeros } state =
			   delayed_seek_count ? in_zeros : begin;
  
  while (nbytes)
    {
      size_t rest = nbytes;

      if (rest < DISKBLOCKSIZE)
	/* Force write */
	state = not_in_zeros;
      else
	{
	  if (buf_all_zeros (buf, rest))
	    {
	      if (state == not_in_zeros)
		{
		  ssize_t bytes = buf - start_ptr + rest;
		  
		  n = write (fildes, start_ptr, bytes);
		  if (n == -1)
		    return -1;
		  nwritten += n;
		  if (n < bytes)
		    return nwritten + seek_count;
		  start_ptr = NULL;
		}
	      else
		seek_count += rest;
	      state = in_zeros;
	    }
	  else
	    {
	      seek_count += delayed_seek_count;
	      if (lseek (fildes, seek_count, SEEK_CUR) == -1)
		return -1;
	      delayed_seek_count = seek_count = 0;
	      state = not_in_zeros;
	      start_ptr = buf;
	    }
	}
      buf += rest;
      nbytes -= rest;
    }

  if (state != in_zeros)
    {
      seek_count += delayed_seek_count;
      if (seek_count && lseek (fildes, seek_count, SEEK_CUR) == -1)
	return -1;
      delayed_seek_count = seek_count = 0;

      n = write (fildes, start_ptr, buf - start_ptr);
      if (n == -1)
	return n;
      nwritten += n;
    }
  delayed_seek_count += seek_count;

  if (flush && delayed_seek_count)
    {
      if (lseek (fildes, delayed_seek_count - 1, SEEK_CUR) == -1)
	return -1;
      n = write (fildes, "", 1);
      if (n != 1)
	return n;
      delayed_seek_count = 0;
    }      
  
  return nwritten + seek_count;
}