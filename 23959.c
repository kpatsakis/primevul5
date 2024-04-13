void sftp_rewind(sftp_file file) {
  file->offset = 0;
  file->eof = 0;
}