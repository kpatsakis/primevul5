unsigned long sftp_tell(sftp_file file) {
  return (unsigned long)file->offset;
}