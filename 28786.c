dump_zeros (struct tar_sparse_file *file, off_t offset)
{
  static char const zero_buf[BLOCKSIZE];

  if (offset < file->offset)
    {
      errno = EINVAL;
      return false;
    }

  while (file->offset < offset)
    {
      size_t size = (BLOCKSIZE < offset - file->offset
		     ? BLOCKSIZE
		     : offset - file->offset);
      ssize_t wrbytes;

      wrbytes = write (file->fd, zero_buf, size);
      if (wrbytes <= 0)
	{
	  if (wrbytes == 0)
	    errno = EINVAL;
	  return false;
	}
      file->offset += wrbytes;
    }

  return true;
}
