char *sftp_canonicalize_path(sftp_session sftp, const char *path) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_string name = NULL;
  ssh_string pathstr;
  ssh_buffer buffer;
  char *cname;
  uint32_t ignored;
  uint32_t id;

  if (sftp == NULL)
    return NULL;
  if (path == NULL) {
    ssh_set_error_invalid(sftp->session, __FUNCTION__);
    return NULL;
  }

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
  if (sftp_packet_write(sftp, SSH_FXP_REALPATH, buffer) < 0) {
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

  if (msg->packet_type == SSH_FXP_NAME) {
    /* we don't care about "count" */
    buffer_get_u32(msg->payload, &ignored);
    /* we only care about the file name string */
    name = buffer_get_ssh_string(msg->payload);
    sftp_message_free(msg);
    if (name == NULL) {
      /* TODO: error message? */
      return NULL;
    }
    cname = ssh_string_to_char(name);
    ssh_string_free(name);
    if (cname == NULL) {
      ssh_set_error_oom(sftp->session);
    }
    return cname;
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