warn_if_file_changed (char *file_name, off_t old_file_size,
		      time_t old_file_mtime)
{
  struct stat new_file_stat;
  if ((*xstat) (file_name, &new_file_stat) < 0)
    {
      stat_error (file_name);
      return;
    }

  /* Only check growth, shrinkage detected in copy_files_disk_to_{disk,tape}()
   */
  if (new_file_stat.st_size > old_file_size)
    error (0, 0,
	   ngettext ("File %s grew, %"PRIuMAX" new byte not copied",
		     "File %s grew, %"PRIuMAX" new bytes not copied",
		     (long)(new_file_stat.st_size - old_file_size)),
	   file_name, (uintmax_t) (new_file_stat.st_size - old_file_size));

  else if (new_file_stat.st_mtime != old_file_mtime)
    error (0, 0, _("File %s was modified while being copied"), file_name);
}