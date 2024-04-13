ssize_t sftp_write(sftp_file file, const void *buf, size_t count) {
  sftp_session sftp = file->sftp;
  sftp_message msg = NULL;
  sftp_status_message status;
  ssh_string datastring;
  ssh_buffer buffer;
  uint32_t id;
  int len;
  int packetlen;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  datastring = ssh_string_new(count);
  if (datastring == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return -1;
  }
  ssh_string_fill(datastring, buf, count);

  id = sftp_get_new_id(file->sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, file->handle) < 0 ||
      buffer_add_u64(buffer, htonll(file->offset)) < 0 ||
      buffer_add_ssh_string(buffer, datastring) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(datastring);
    return -1;
  }
  ssh_string_free(datastring);
  packetlen=buffer_get_rest_len(buffer);
  len = sftp_packet_write(file->sftp, SSH_FXP_WRITE, buffer);
  ssh_buffer_free(buffer);
  if (len < 0) {
    return -1;
  } else  if (len != packetlen) {
    ssh_log(sftp->session, SSH_LOG_PACKET,
        "Could not write as much data as expected");
  }

  while (msg == NULL) {
    if (sftp_read_and_dispatch(file->sftp) < 0) {
      /* something nasty has happened */
      return -1;
    }
    msg = sftp_dequeue(file->sftp, id);
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
        case SSH_FX_OK:
          file->offset += count;
          status_msg_free(status);
          return count;
        default:
          break;
      }
      ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
          "SFTP server: %s", status->errormsg);
      file->offset += count;
      status_msg_free(status);
      return -1;
    default:
      ssh_set_error(sftp->session, SSH_FATAL,
          "Received message %d during write!", msg->packet_type);
      sftp_message_free(msg);
      return -1;
  }

  return -1; /* not reached */
}