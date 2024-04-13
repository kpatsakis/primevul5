ssize_t sftp_read(sftp_file handle, void *buf, size_t count) {
  sftp_session sftp = handle->sftp;
  sftp_message msg = NULL;
  sftp_status_message status;
  ssh_string datastring;
  ssh_buffer buffer;
  int id;

  if (handle->eof) {
    return 0;
  }

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }
  id = sftp_get_new_id(handle->sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, handle->handle) < 0 ||
      buffer_add_u64(buffer, htonll(handle->offset)) < 0 ||
      buffer_add_u32(buffer,htonl(count)) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }
  if (sftp_packet_write(handle->sftp, SSH_FXP_READ, buffer) < 0) {
    ssh_buffer_free(buffer);
    return -1;
  }
  ssh_buffer_free(buffer);

  while (msg == NULL) {
    if (handle->nonblocking) {
      if (ssh_channel_poll(handle->sftp->channel, 0) == 0) {
        /* we cannot block */
        return 0;
      }
    }
    if (sftp_read_and_dispatch(handle->sftp) < 0) {
      /* something nasty has happened */
      return -1;
    }
    msg = sftp_dequeue(handle->sftp, id);
  }

  switch (msg->packet_type) {
    case SSH_FXP_STATUS:
      status = parse_status_msg(msg);
      sftp_message_free(msg);
      if (status == NULL) {
        return -1;
      }
      sftp_set_error(sftp, status->status);
      switch (status->status) {
        case SSH_FX_EOF:
          handle->eof = 1;
          status_msg_free(status);
          return 0;
        default:
          break;
      }
      ssh_set_error(sftp->session,SSH_REQUEST_DENIED,
          "SFTP server: %s", status->errormsg);
      status_msg_free(status);
      return -1;
    case SSH_FXP_DATA:
      datastring = buffer_get_ssh_string(msg->payload);
      sftp_message_free(msg);
      if (datastring == NULL) {
        ssh_set_error(sftp->session, SSH_FATAL,
            "Received invalid DATA packet from sftp server");
        return -1;
      }

      if (ssh_string_len(datastring) > count) {
        ssh_set_error(sftp->session, SSH_FATAL,
            "Received a too big DATA packet from sftp server: "
            "%" PRIdS " and asked for %" PRIdS,
            ssh_string_len(datastring), count);
        ssh_string_free(datastring);
        return -1;
      }
      count = ssh_string_len(datastring);
      handle->offset += count;
      memcpy(buf, ssh_string_data(datastring), count);
      ssh_string_free(datastring);
      return count;
    default:
      ssh_set_error(sftp->session, SSH_FATAL,
          "Received message %d during read!", msg->packet_type);
      sftp_message_free(msg);
      return -1;
  }

  return -1; /* not reached */
}