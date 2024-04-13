uint64_t sftp_tell64(sftp_file file) {
  return (uint64_t) file->offset;
}