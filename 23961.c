void sftp_statvfs_free(sftp_statvfs_t statvfs) {
  if (statvfs == NULL) {
    return;
  }

  SAFE_FREE(statvfs);
}