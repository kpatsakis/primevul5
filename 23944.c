int sftp_close(sftp_file file){
  int err = SSH_NO_ERROR;

  SAFE_FREE(file->name);
  if (file->handle){
    err = sftp_handle_close(file->sftp,file->handle);
    ssh_string_free(file->handle);
  }
  /* FIXME: check server response and implement errno */
  SAFE_FREE(file);

  return err;
}