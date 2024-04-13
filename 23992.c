int sftp_symlink(sftp_session sftp, const char *target, const char *dest) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_string target_s;
  ssh_string dest_s;
  ssh_buffer buffer;
  uint32_t id;

  if (sftp == NULL)
    return -1;
  if (target == NULL || dest == NULL) {
    ssh_set_error_invalid(sftp->session, __FUNCTION__);
    return -1;
  }

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  target_s = ssh_string_from_char(target);
  if (target_s == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }

  dest_s = ssh_string_from_char(dest);
  if (dest_s == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_string_free(target_s);
    ssh_buffer_free(buffer);
    return -1;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(dest_s);
    ssh_string_free(target_s);
    return -1;
  }
  if (ssh_get_openssh_version(sftp->session)) {
    /* TODO check for version number if they ever fix it. */
    if (buffer_add_ssh_string(buffer, target_s) < 0 ||
      buffer_add_ssh_string(buffer, dest_s) < 0) {
      ssh_set_error_oom(sftp->session);
      ssh_buffer_free(buffer);
      ssh_string_free(dest_s);
      ssh_string_free(target_s);
      return -1;
    }
  } else {
    if (buffer_add_ssh_string(buffer, dest_s) < 0 ||
      buffer_add_ssh_string(buffer, target_s) < 0) {
      ssh_set_error_oom(sftp->session);
      ssh_buffer_free(buffer);
      ssh_string_free(dest_s);
      ssh_string_free(target_s);
      return -1;
    }
  }

  if (sftp_packet_write(sftp, SSH_FXP_SYMLINK, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(dest_s);
    ssh_string_free(target_s);
    return -1;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(dest_s);
  ssh_string_free(target_s);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      return -1;
    }
    msg = sftp_dequeue(sftp, id);
  }

  /* By specification, this command only returns SSH_FXP_STATUS */
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
    ssh_set_error(sftp->session, SSH_FATAL,
        "Received message %d when attempting to set stats", msg->packet_type);
    sftp_message_free(msg);
  }

  return -1;
}