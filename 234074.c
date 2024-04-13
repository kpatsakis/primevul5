set_copypass_perms (int fd, const char *name, struct stat *st)
{
  struct cpio_file_stat header;
  header.c_name = (char*)name;
  stat_to_cpio (&header, st);
  set_perms (fd, &header);
}