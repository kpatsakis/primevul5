cpio_mkdir (struct cpio_file_stat *file_hdr, int *setstat_delayed)
{
  int rc;
  mode_t mode = file_hdr->c_mode;
  
  if (!(file_hdr->c_mode & S_IWUSR))
    {
      rc = mkdir (file_hdr->c_name, mode | S_IWUSR);
      if (rc == 0)
	{
	  delay_cpio_set_stat (file_hdr, 0);
	  *setstat_delayed = 1;
	}
    }
  else
    {
      rc = mkdir (file_hdr->c_name, mode);
      *setstat_delayed = 0;
    }
  return rc;
}