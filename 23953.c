sftp_attributes sftp_lstat(sftp_session session, const char *path) {
  return sftp_xstat(session, path, SSH_FXP_LSTAT);
}