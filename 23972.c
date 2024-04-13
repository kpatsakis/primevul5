int sftp_rename(sftp_session sftp, const char *original, const char *newname) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_buffer buffer;
  ssh_string oldpath;
  ssh_string newpath;
  uint32_t id;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  oldpath = ssh_string_from_char(original);
  if (oldpath == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }

  newpath = ssh_string_from_char(newname);
  if (newpath == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(oldpath);
    return -1;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, oldpath) < 0 ||
      buffer_add_ssh_string(buffer, newpath) < 0 ||
      /* POSIX rename atomically replaces newpath, we should do the same
       * only available on >=v4 */
      sftp->version>=4 ? (buffer_add_u32(buffer, SSH_FXF_RENAME_OVERWRITE) < 0):0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(oldpath);
    ssh_string_free(newpath);
    return -1;
  }
  if (sftp_packet_write(sftp, SSH_FXP_RENAME, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(oldpath);
    ssh_string_free(newpath);
    return -1;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(oldpath);
  ssh_string_free(newpath);

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
     * Status should be SSH_FX_OK if the command was successful, if it didn't,
     * then there was an error
     */
    ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
        "SFTP server: %s", status->errormsg);
    status_msg_free(status);
    return -1;
  } else {
    ssh_set_error(sftp->session, SSH_FATAL,
        "Received message %d when attempting to rename",
        msg->packet_type);
    sftp_message_free(msg);
  }

  return -1;
}