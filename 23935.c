int sftp_utimes(sftp_session sftp, const char *file,
    const struct timeval *times) {
	struct sftp_attributes_struct attr;
  ZERO_STRUCT(attr);

  attr.atime = times[0].tv_sec;
  attr.atime_nseconds = times[0].tv_usec;

  attr.mtime = times[1].tv_sec;
  attr.mtime_nseconds = times[1].tv_usec;

  attr.flags |= SSH_FILEXFER_ATTR_ACCESSTIME | SSH_FILEXFER_ATTR_MODIFYTIME |
    SSH_FILEXFER_ATTR_SUBSECOND_TIMES;

  return sftp_setstat(sftp, file, &attr);
}