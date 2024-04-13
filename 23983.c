sftp_attributes sftp_fstat(sftp_file file) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_buffer buffer;
  uint32_t id;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(file->sftp->session);
    return NULL;
  }

  id = sftp_get_new_id(file->sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, file->handle) < 0) {
    ssh_set_error_oom(file->sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }
  if (sftp_packet_write(file->sftp, SSH_FXP_FSTAT, buffer) < 0) {
    ssh_buffer_free(buffer);
    return NULL;
  }
  ssh_buffer_free(buffer);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(file->sftp) < 0) {
      return NULL;
    }
    msg = sftp_dequeue(file->sftp, id);
  }

  if (msg->packet_type == SSH_FXP_ATTRS){
    return sftp_parse_attr(file->sftp, msg->payload, 0);
  } else if (msg->packet_type == SSH_FXP_STATUS) {
    status = parse_status_msg(msg);
    sftp_message_free(msg);
    if (status == NULL) {
      return NULL;
    }
    ssh_set_error(file->sftp->session, SSH_REQUEST_DENIED,
        "SFTP server: %s", status->errormsg);
    status_msg_free(status);

    return NULL;
  }
  ssh_set_error(file->sftp->session, SSH_FATAL,
      "Received msg %d during fstat()", msg->packet_type);
  sftp_message_free(msg);

  return NULL;
}