int sftp_seek64(sftp_file file, uint64_t new_offset) {
  if (file == NULL) {
    return -1;
  }

  file->offset = new_offset;
  file->eof = 0;

  return 0;
}