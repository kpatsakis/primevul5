int sftp_closedir(sftp_dir dir){
  int err = SSH_NO_ERROR;

  SAFE_FREE(dir->name);
  if (dir->handle) {
    err = sftp_handle_close(dir->sftp, dir->handle);
    ssh_string_free(dir->handle);
  }
  /* FIXME: check server response and implement errno */
  ssh_buffer_free(dir->buffer);
  SAFE_FREE(dir);

  return err;
}