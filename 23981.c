sftp_attributes sftp_readdir(sftp_session sftp, sftp_dir dir) {
  sftp_message msg = NULL;
  sftp_status_message status;
  sftp_attributes attr;
  ssh_buffer payload;
  uint32_t id;

  if (dir->buffer == NULL) {
    payload = ssh_buffer_new();
    if (payload == NULL) {
      ssh_set_error_oom(sftp->session);
      return NULL;
    }

    id = sftp_get_new_id(sftp);
    if (buffer_add_u32(payload, id) < 0 ||
        buffer_add_ssh_string(payload, dir->handle) < 0) {
      ssh_set_error_oom(sftp->session);
      ssh_buffer_free(payload);
      return NULL;
    }

    if (sftp_packet_write(sftp, SSH_FXP_READDIR, payload) < 0) {
      ssh_buffer_free(payload);
      return NULL;
    }
    ssh_buffer_free(payload);

    ssh_log(sftp->session, SSH_LOG_PACKET,
        "Sent a ssh_fxp_readdir with id %d", id);

    while (msg == NULL) {
      if (sftp_read_and_dispatch(sftp) < 0) {
        /* something nasty has happened */
        return NULL;
      }
      msg = sftp_dequeue(sftp, id);
    }

    switch (msg->packet_type){
      case SSH_FXP_STATUS:
        status = parse_status_msg(msg);
        sftp_message_free(msg);
        if (status == NULL) {
          return NULL;
        }
        sftp_set_error(sftp, status->status);
        switch (status->status) {
          case SSH_FX_EOF:
            dir->eof = 1;
            status_msg_free(status);
            return NULL;
          default:
            break;
        }

        ssh_set_error(sftp->session, SSH_FATAL,
            "Unknown error status: %d", status->status);
        status_msg_free(status);

        return NULL;
      case SSH_FXP_NAME:
        buffer_get_u32(msg->payload, &dir->count);
        dir->count = ntohl(dir->count);
        dir->buffer = msg->payload;
        msg->payload = NULL;
        sftp_message_free(msg);
        break;
      default:
        ssh_set_error(sftp->session, SSH_FATAL,
            "Unsupported message back %d", msg->packet_type);
        sftp_message_free(msg);

        return NULL;
    }
  }

  /* now dir->buffer contains a buffer and dir->count != 0 */
  if (dir->count == 0) {
    ssh_set_error(sftp->session, SSH_FATAL,
        "Count of files sent by the server is zero, which is invalid, or "
        "libsftp bug");
    return NULL;
  }

  ssh_log(sftp->session, SSH_LOG_RARE, "Count is %d", dir->count);

  attr = sftp_parse_attr(sftp, dir->buffer, 1);
  if (attr == NULL) {
    ssh_set_error(sftp->session, SSH_FATAL,
        "Couldn't parse the SFTP attributes");
    return NULL;
  }

  dir->count--;
  if (dir->count == 0) {
    ssh_buffer_free(dir->buffer);
    dir->buffer = NULL;
  }

  return attr;
}