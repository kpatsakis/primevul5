int sftp_get_error(sftp_session sftp) {
  if (sftp == NULL) {
    return -1;
  }

  return sftp->errnum;
}