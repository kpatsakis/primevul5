writeout_other_defers (struct cpio_file_stat *file_hdr, int out_des)
{
  struct deferment *d;
  struct deferment *d_prev;
  ino_t	ino;
  int 	maj;
  int   min;
  ino = file_hdr->c_ino;
  maj = file_hdr->c_dev_maj;
  min = file_hdr->c_dev_min;
  d_prev = NULL;
  d = deferouts;
  while (d != NULL)
    {
      if ( (d->header.c_ino == ino) && (d->header.c_dev_maj == maj)
	  && (d->header.c_dev_min == min) )
	{
	  struct deferment *d_free;
	  d->header.c_filesize = 0;
	  write_out_header (&d->header, out_des);
	  if (d_prev != NULL)
	    d_prev->next = d->next;
	  else
	    deferouts = d->next;
	  d_free = d;
	  d = d->next;
	  free_deferment (d_free);
	}
      else
	{
	  d_prev = d;
	  d = d->next;
	}
    }
  return;
}