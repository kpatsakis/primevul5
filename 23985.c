int sftp_unlink(sftp_session sftp, const char *file) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_string filename;
  ssh_buffer buffer;
  uint32_t id;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  filename = ssh_string_from_char(file);
  if (filename == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, filename) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(filename);
    return -1;
  }
  if (sftp_packet_write(sftp, SSH_FXP_REMOVE, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(filename);
    return -1;
  }
  ssh_string_free(filename);
  ssh_buffer_free(buffer);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp)) {
      return -1;
    }
    msg = sftp_dequeue(sftp, id);
  }

  if (msg->packet_type == SSH_FXP_STATUS) {
    /* by specification, this command's only supposed to return SSH_FXP_STATUS */
    status = parse_status_msg(msg);
    sftp_message_free(msg);
    if (status == NULL) {
      return -1;
    }
    sftp_set_error(sftp, status->status);
    switch (status->status) {
      case SSH_FX_OK:
        status_msg_free(status);
        return 0;
      default:
        break;
    }

    /*
     * The status should be SSH_FX_OK if the command was successful, if it
     * didn't, then there was an error
     */
    ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
        "SFTP server: %s", status->errormsg);
    status_msg_free(status);
    return -1;
  } else {
    ssh_set_error(sftp->session,SSH_FATAL,
        "Received message %d when attempting to remove file", msg->packet_type);
    sftp_message_free(msg);
  }

  return -1;
}