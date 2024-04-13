delay_set_stat (char const *file_name, struct stat *st,
		mode_t invert_permissions)
{
  struct cpio_file_stat fs;

  stat_to_cpio (&fs, st);
  fs.c_name = (char*) file_name;
  delay_cpio_set_stat (&fs, invert_permissions);
}