char *sftp_readlink(sftp_session sftp, const char *path) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_string path_s = NULL;
  ssh_string link_s = NULL;
  ssh_buffer buffer;
  char *lnk;
  uint32_t ignored;
  uint32_t id;

  if (sftp == NULL)
    return NULL;
  if (path == NULL) {
    ssh_set_error_invalid(sftp, __FUNCTION__);
    return NULL;
  }
  if (sftp->version < 3){
    ssh_set_error(sftp,SSH_REQUEST_DENIED,"sftp version %d does not support sftp_readlink",sftp->version);
    return NULL;
  }
  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }

  path_s = ssh_string_from_char(path);
  if (path_s == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, path_s) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(path_s);
    return NULL;
  }
  if (sftp_packet_write(sftp, SSH_FXP_READLINK, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(path_s);
    return NULL;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(path_s);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      return NULL;
    }
    msg = sftp_dequeue(sftp, id);
  }

  if (msg->packet_type == SSH_FXP_NAME) {
    /* we don't care about "count" */
    buffer_get_u32(msg->payload, &ignored);
    /* we only care about the file name string */
    link_s = buffer_get_ssh_string(msg->payload);
    sftp_message_free(msg);
    if (link_s == NULL) {
      /* TODO: what error to set here? */
      return NULL;
    }
    lnk = ssh_string_to_char(link_s);
    ssh_string_free(link_s);

    return lnk;
  } else if (msg->packet_type == SSH_FXP_STATUS) { /* bad response (error) */
    status = parse_status_msg(msg);
    sftp_message_free(msg);
    if (status == NULL) {
      return NULL;
    }
    ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
        "SFTP server: %s", status->errormsg);
    status_msg_free(status);
  } else { /* this shouldn't happen */
    ssh_set_error(sftp->session, SSH_FATAL,
        "Received message %d when attempting to set stats", msg->packet_type);
    sftp_message_free(msg);
  }

  return NULL;
}