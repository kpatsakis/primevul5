writeout_final_defers (int out_des)
{
  struct deferment *d;
  int other_count;
  while (deferouts != NULL)
    {
      d = deferouts;
      other_count = count_defered_links_to_dev_ino (&d->header);
      if (other_count == 1)
	{
	  writeout_defered_file (&d->header, out_des);
	}
      else
	{
	  struct cpio_file_stat file_hdr;
	  file_hdr = d->header;
	  file_hdr.c_filesize = 0;
	  write_out_header (&file_hdr, out_des);
	}
      deferouts = deferouts->next;
    }
}