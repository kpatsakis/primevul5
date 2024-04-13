count_defered_links_to_dev_ino (struct cpio_file_stat *file_hdr)
{
  struct deferment *d;
  ino_t	ino = file_hdr->c_ino;
  long 	maj = file_hdr->c_dev_maj;
  long  min = file_hdr->c_dev_min;
  size_t count = 0;

  for (d = deferouts; d != NULL; d = d->next)
    {
      if (d->header.c_ino == ino
	  && d->header.c_dev_maj == maj
	  && d->header.c_dev_min == min)
	++count;
    }
  return count;
}