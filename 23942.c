static sftp_ext sftp_ext_new(void) {
  sftp_ext ext;

  ext = malloc(sizeof(struct sftp_ext_struct));
  if (ext == NULL) {
    return NULL;
  }
  ZERO_STRUCTP(ext);

  return ext;
}