static sftp_attributes sftp_xstat(sftp_session sftp, const char *path,
    int param) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_string pathstr;
  ssh_buffer buffer;
  uint32_t id;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }

  pathstr = ssh_string_from_char(path);
  if (pathstr == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, pathstr) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(pathstr);
    return NULL;
  }
  if (sftp_packet_write(sftp, param, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(pathstr);
    return NULL;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(pathstr);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      return NULL;
    }
    msg = sftp_dequeue(sftp, id);
  }

  if (msg->packet_type == SSH_FXP_ATTRS) {
    sftp_attributes attr = sftp_parse_attr(sftp, msg->payload, 0);
    sftp_message_free(msg);

    return attr;
  } else if (msg->packet_type == SSH_FXP_STATUS) {
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
  }
  ssh_set_error(sftp->session, SSH_FATAL,
      "Received mesg %d during stat()", msg->packet_type);
  sftp_message_free(msg);

  return NULL;
}