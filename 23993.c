sftp_statvfs_t sftp_statvfs(sftp_session sftp, const char *path) {
  sftp_status_message status = NULL;
  sftp_message msg = NULL;
  ssh_string pathstr;
  ssh_string ext;
  ssh_buffer buffer;
  uint32_t id;

  if (sftp == NULL)
    return NULL;
  if (path == NULL) {
    ssh_set_error_invalid(sftp->session, __FUNCTION__);
    return NULL;
  }
  if (sftp->version < 3){
    ssh_set_error(sftp,SSH_REQUEST_DENIED,"sftp version %d does not support sftp_statvfs",sftp->version);
    return NULL;
  }

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }

  ext = ssh_string_from_char("statvfs@openssh.com");
  if (ext == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }

  pathstr = ssh_string_from_char(path);
  if (pathstr == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(ext);
    return NULL;
  }

  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, ext) < 0 ||
      buffer_add_ssh_string(buffer, pathstr) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(ext);
    ssh_string_free(pathstr);
    return NULL;
  }
  if (sftp_packet_write(sftp, SSH_FXP_EXTENDED, buffer) < 0) {
    ssh_buffer_free(buffer);
    ssh_string_free(ext);
    ssh_string_free(pathstr);
    return NULL;
  }
  ssh_buffer_free(buffer);
  ssh_string_free(ext);
  ssh_string_free(pathstr);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      return NULL;
    }
    msg = sftp_dequeue(sftp, id);
  }

  if (msg->packet_type == SSH_FXP_EXTENDED_REPLY) {
  	sftp_statvfs_t  buf = sftp_parse_statvfs(sftp, msg->payload);
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
        "Received message %d when attempting to get statvfs", msg->packet_type);
    sftp_message_free(msg);
  }

  return NULL;
}