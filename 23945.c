int sftp_rmdir(sftp_session sftp, const char *directory) {
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

  filename = ssh_string_from_char(directory);
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
  if (sftp_packet_write(sftp, SSH_FXP_RMDIR, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(filename);
    return -1;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(filename);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      return -1;
    }
    msg = sftp_dequeue(sftp, id);
  }

  /* By specification, this command returns SSH_FXP_STATUS */
  if (msg->packet_type == SSH_FXP_STATUS) {
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
        break;
      default:
        break;
    }
    ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
        "SFTP server: %s", status->errormsg);
    status_msg_free(status);
    return -1;
  } else {
    ssh_set_error(sftp->session, SSH_FATAL,
        "Received message %d when attempting to remove directory",
        msg->packet_type);
    sftp_message_free(msg);
  }

  return -1;
}