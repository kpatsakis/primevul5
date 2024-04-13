sftp_dir sftp_opendir(sftp_session sftp, const char *path){
  sftp_message msg = NULL;
  sftp_file file = NULL;
  sftp_dir dir = NULL;
  sftp_status_message status;
  ssh_string path_s;
  ssh_buffer payload;
  uint32_t id;

  payload = ssh_buffer_new();
  if (payload == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }

  path_s = ssh_string_from_char(path);
  if (path_s == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(payload);
    return NULL;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(payload, id) < 0 ||
      buffer_add_ssh_string(payload, path_s) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(payload);
    ssh_string_free(path_s);
    return NULL;
  }
  ssh_string_free(path_s);

  if (sftp_packet_write(sftp, SSH_FXP_OPENDIR, payload) < 0) {
    ssh_buffer_free(payload);
    return NULL;
  }
  ssh_buffer_free(payload);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      /* something nasty has happened */
      return NULL;
    }
    msg = sftp_dequeue(sftp, id);
  }

  switch (msg->packet_type) {
    case SSH_FXP_STATUS:
      status = parse_status_msg(msg);
      sftp_message_free(msg);
      if (status == NULL) {
        return NULL;
      }
      sftp_set_error(sftp, status->status);
      ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
          "SFTP server: %s", status->errormsg);
      status_msg_free(status);
      return NULL;
    case SSH_FXP_HANDLE:
      file = parse_handle_msg(msg);
      sftp_message_free(msg);
      if (file != NULL) {
        dir = malloc(sizeof(struct sftp_dir_struct));
        if (dir == NULL) {
          ssh_set_error_oom(sftp->session);
          return NULL;
        }
        ZERO_STRUCTP(dir);

        dir->sftp = sftp;
        dir->name = strdup(path);
        if (dir->name == NULL) {
          SAFE_FREE(dir);
          SAFE_FREE(file);
          return NULL;
        }
        dir->handle = file->handle;
        SAFE_FREE(file);
      }
      return dir;
    default:
      ssh_set_error(sftp->session, SSH_FATAL,
          "Received message %d during opendir!", msg->packet_type);
      sftp_message_free(msg);
  }

  return NULL;
}