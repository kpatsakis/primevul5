int sftp_chmod(sftp_session sftp, const char *file, mode_t mode) {
	struct sftp_attributes_struct attr;
  ZERO_STRUCT(attr);
  attr.permissions = mode;
  attr.flags = SSH_FILEXFER_ATTR_PERMISSIONS;

  return sftp_setstat(sftp, file, &attr);
}