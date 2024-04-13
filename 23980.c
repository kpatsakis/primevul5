int sftp_chown(sftp_session sftp, const char *file, uid_t owner, gid_t group) {
	struct sftp_attributes_struct attr;
  ZERO_STRUCT(attr);

  attr.uid = owner;
  attr.gid = group;

  attr.flags = SSH_FILEXFER_ATTR_UIDGID;

  return sftp_setstat(sftp, file, &attr);
}