sftp_statvfs_t sftp_fstatvfs(sftp_file file) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  sftp_session sftp;
  ssh_string ext;
  ssh_buffer buffer;
  uint32_t id;

  if (file == NULL) {
    return NULL;
  }
  sftp = file->sftp;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }

  ext = ssh_string_from_char("fstatvfs@openssh.com");
  if (ext == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, ext) < 0 ||
      buffer_add_ssh_string(buffer, file->handle) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(ext);
    return NULL;
  }
  if (sftp_packet_write(sftp, SSH_FXP_EXTENDED, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(ext);
    return NULL;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(ext);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      return NULL;
    }
    msg = sftp_dequeue(sftp, id);
  }

  if (msg->packet_type == SSH_FXP_EXTENDED_REPLY) {
  	sftp_statvfs_t buf = sftp_parse_statvfs(sftp, msg->payload);
    sftp_message_free(msg);
    if (buf == NULL) {
      return NULL;
    }

    return buf;
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