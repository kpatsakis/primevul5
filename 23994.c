sftp_attributes sftp_stat(sftp_session session, const char *path) {
  return sftp_xstat(session, path, SSH_FXP_STAT);
}