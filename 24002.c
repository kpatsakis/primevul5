int sftp_extension_supported(sftp_session sftp, const char *name,
    const char *data) {
  int i, n;

  n = sftp_extensions_get_count(sftp);
  for (i = 0; i < n; i++) {
    if (strcmp(sftp_extensions_get_name(sftp, i), name) == 0 &&
        strcmp(sftp_extensions_get_data(sftp, i), data) == 0) {
      return 1;
    }
  }

  return 0;
}