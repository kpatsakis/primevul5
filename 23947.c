static sftp_file parse_handle_msg(sftp_message msg){
  sftp_file file;

  if(msg->packet_type != SSH_FXP_HANDLE) {
    ssh_set_error(msg->sftp->session, SSH_FATAL,
        "Not a ssh_fxp_handle message passed in!");
    return NULL;
  }

  file = malloc(sizeof(struct sftp_file_struct));
  if (file == NULL) {
    ssh_set_error_oom(msg->sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(file);

  file->handle = buffer_get_ssh_string(msg->payload);
  if (file->handle == NULL) {
    ssh_set_error(msg->sftp->session, SSH_FATAL,
        "Invalid SSH_FXP_HANDLE message");
    SAFE_FREE(file);
    return NULL;
  }

  file->sftp = msg->sftp;
  file->offset = 0;
  file->eof = 0;

  return file;
}