static void sftp_ext_free(sftp_ext ext) {
  unsigned int i;

  if (ext == NULL) {
    return;
  }

  if (ext->count) {
    for (i = 0; i < ext->count; i++) {
      SAFE_FREE(ext->name[i]);
      SAFE_FREE(ext->data[i]);
    }
    SAFE_FREE(ext->name);
    SAFE_FREE(ext->data);
  }

  SAFE_FREE(ext);
}