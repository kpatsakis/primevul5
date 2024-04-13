static void sftp_set_error(sftp_session sftp, int errnum) {
  if (sftp != NULL) {
    sftp->errnum = errnum;
  }
}