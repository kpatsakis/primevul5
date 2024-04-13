unsigned int sftp_extensions_get_count(sftp_session sftp) {
  if (sftp == NULL || sftp->ext == NULL) {
    return 0;
  }

  return sftp->ext->count;
}