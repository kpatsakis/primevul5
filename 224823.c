close_all_files ()
{
  register int i, fd_table_size;

  fd_table_size = getdtablesize ();
  if (fd_table_size > 256)	/* clamp to a reasonable value */
    fd_table_size = 256;

  for (i = 3; i < fd_table_size; i++)
    close (i);
}