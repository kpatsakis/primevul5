set_perms (int fd, struct cpio_file_stat *header)
{
  if (!no_chown_flag)
    {
      uid_t uid = CPIO_UID (header->c_uid);
      gid_t gid = CPIO_GID (header->c_gid); 
      if ((fchown_or_chown (fd, header->c_name, uid, gid) < 0)
	  && errno != EPERM)
	chown_error_details (header->c_name, uid, gid);
    }
  /* chown may have turned off some permissions we wanted. */
  if (fchmod_or_chmod (fd, header->c_name, header->c_mode) < 0)
    chmod_error_details (header->c_name, header->c_mode);
  if (retain_time_flag)
    set_file_times (fd, header->c_name, header->c_mtime, header->c_mtime);
}