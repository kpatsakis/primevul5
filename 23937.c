const char *sftp_extensions_get_name(sftp_session sftp, unsigned int idx) {
  if (sftp == NULL)
    return NULL;
  if (sftp->ext == NULL || sftp->ext->name == NULL) {
    ssh_set_error_invalid(sftp->session, __FUNCTION__);
    return NULL;
  }

  if (idx > sftp->ext->count) {
    ssh_set_error_invalid(sftp->session, __FUNCTION__);
    return NULL;
  }

  return sftp->ext->name[idx];
}