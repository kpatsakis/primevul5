static int sftp_handle_close(sftp_session sftp, ssh_string handle) {
  sftp_status_message status;
  sftp_message msg = NULL;
  ssh_buffer buffer = NULL;
  uint32_t id;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, handle) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }
  if (sftp_packet_write(sftp, SSH_FXP_CLOSE ,buffer) < 0) {
    ssh_buffer_free(buffer);
    return -1;
  }
  ssh_buffer_free(buffer);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      /* something nasty has happened */
      return -1;
    }
    msg = sftp_dequeue(sftp,id);
  }

  switch (msg->packet_type) {
    case SSH_FXP_STATUS:
      status = parse_status_msg(msg);
      sftp_message_free(msg);
      if(status == NULL) {
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
    default:
      ssh_set_error(sftp->session, SSH_FATAL,
          "Received message %d during sftp_handle_close!", msg->packet_type);
      sftp_message_free(msg);
  }

  return -1;
}