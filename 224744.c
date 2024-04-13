new_fd_bitmap (size)
     int size;
{
  struct fd_bitmap *ret;

  ret = (struct fd_bitmap *)xmalloc (sizeof (struct fd_bitmap));

  ret->size = size;

  if (size)
    {
      ret->bitmap = (char *)xmalloc (size);
      memset (ret->bitmap, '\0', size);
    }
  else
    ret->bitmap = (char *)NULL;
  return (ret);
}