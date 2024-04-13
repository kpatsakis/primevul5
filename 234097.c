create_defered_links_to_skipped (struct cpio_file_stat *file_hdr,
				 int in_file_des)
{
  struct deferment *d;
  struct deferment *d_prev;
  ino_t	ino;
  int 	maj;
  int   min;
  if (file_hdr->c_filesize == 0)
    {
      /* The file doesn't have any data attached to it so we don't have
         to bother.  */
      return -1;
    }
  ino = file_hdr->c_ino;
  maj = file_hdr->c_dev_maj;
  min = file_hdr->c_dev_min;
  d = deferments;
  d_prev = NULL;
  while (d != NULL)
    {
      if ( (d->header.c_ino == ino) && (d->header.c_dev_maj == maj)
	  && (d->header.c_dev_min == min) )
	{
	  if (d_prev != NULL)
	    d_prev->next = d->next;
	  else
	    deferments = d->next;
	  cpio_set_c_name (file_hdr, d->header.c_name);
	  free_deferment (d);
	  copyin_regular_file(file_hdr, in_file_des);
	  return 0;
	}
      else
	{
	  d_prev = d;
	  d = d->next;
	}
    }
  return -1;
}