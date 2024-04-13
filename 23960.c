int sftp_packet_write(sftp_session sftp, uint8_t type, ssh_buffer payload){
  int size;

  if (buffer_prepend_data(payload, &type, sizeof(uint8_t)) < 0) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  size = htonl(buffer_get_rest_len(payload));
  if (buffer_prepend_data(payload, &size, sizeof(uint32_t)) < 0) {
    ssh_set_error_oom(sftp->session);
    return -1;
  }

  size = ssh_channel_write(sftp->channel, buffer_get_rest(payload),
      buffer_get_rest_len(payload));
  if (size < 0) {
    return -1;
  } else if((uint32_t) size != buffer_get_rest_len(payload)) {
    ssh_log(sftp->session, SSH_LOG_PACKET,
        "Had to write %d bytes, wrote only %d",
        buffer_get_rest_len(payload),
        size);
  }

  return size;
}