open_archive (char *file)
{
  int fd;
  void (*copy_in) ();		/* Workaround for pcc bug.  */

  copy_in = process_copy_in;

  if (copy_function == copy_in)
    fd = rmtopen (file, O_RDONLY | O_BINARY, MODE_RW, rsh_command_option);
  else
    {
      if (!append_flag)
	fd = rmtopen (file, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, MODE_RW,
			rsh_command_option);
      else
	fd = rmtopen (file, O_RDWR | O_BINARY, MODE_RW, rsh_command_option);
    }

  return fd;
}