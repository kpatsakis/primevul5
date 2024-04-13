int sftp_async_read_begin(sftp_file file, uint32_t len){
  sftp_session sftp = file->sftp;
  ssh_buffer buffer;
  uint32_t id;

  sftp_enter_function();

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, file->handle) < 0 ||
      buffer_add_u64(buffer, htonll(file->offset)) < 0 ||
      buffer_add_u32(buffer, htonl(len)) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }
  if (sftp_packet_write(sftp, SSH_FXP_READ, buffer) < 0) {
    ssh_buffer_free(buffer);
    return -1;
  }
  ssh_buffer_free(buffer);

  file->offset += len; /* assume we'll read len bytes */

  sftp_leave_function();
  return id;
}