read_for_checksum (int in_file_des, int file_size, char *file_name)
{
  uint32_t crc;
  char buf[BUFSIZ];
  int bytes_left;
  int bytes_read;
  int i;

  crc = 0;

  for (bytes_left = file_size; bytes_left > 0; bytes_left -= bytes_read)
    {
      bytes_read = read (in_file_des, buf, BUFSIZ);
      if (bytes_read < 0)
	error (PAXEXIT_FAILURE, errno, _("cannot read checksum for %s"), file_name);
      if (bytes_read == 0)
	break;
      if (bytes_left < bytes_read)
        bytes_read = bytes_left;
      for (i = 0; i < bytes_read; ++i)
	crc += buf[i] & 0xff;
    }
  if (lseek (in_file_des, 0L, SEEK_SET))
    error (PAXEXIT_FAILURE, errno, _("cannot read checksum for %s"), file_name);

  return crc;
}