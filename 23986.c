sftp_packet sftp_packet_read(sftp_session sftp) {
  unsigned char buffer[4096];
  sftp_packet packet = NULL;
  uint32_t size;
  int r;

  sftp_enter_function();

  packet = malloc(sizeof(struct sftp_packet_struct));
  if (packet == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }
  packet->sftp = sftp;
  packet->payload = ssh_buffer_new();
  if (packet->payload == NULL) {
    ssh_set_error_oom(sftp->session);
    SAFE_FREE(packet);
    return NULL;
  }

  r=ssh_channel_read(sftp->channel, buffer, 4, 0);
  if (r < 0) {
    ssh_buffer_free(packet->payload);
    SAFE_FREE(packet);
    sftp_leave_function();
    return NULL;
  }
  buffer_add_data(packet->payload,buffer, r);
  if (buffer_get_u32(packet->payload, &size) != sizeof(uint32_t)) {
    ssh_set_error(sftp->session, SSH_FATAL, "Short sftp packet!");
    ssh_buffer_free(packet->payload);
    SAFE_FREE(packet);
    sftp_leave_function();
    return NULL;
  }

  size = ntohl(size);
  r=ssh_channel_read(sftp->channel, buffer, 1, 0);
  if (r <= 0) {
    /* TODO: check if there are cases where an error needs to be set here */
    ssh_buffer_free(packet->payload);
    SAFE_FREE(packet);
    sftp_leave_function();
    return NULL;
  }
  buffer_add_data(packet->payload, buffer, r);
  buffer_get_u8(packet->payload, &packet->type);
  size=size-1;
  while (size>0){
    r=ssh_channel_read(sftp->channel,buffer,
        sizeof(buffer)>size ? size:sizeof(buffer),0);

    if(r <= 0) {
      /* TODO: check if there are cases where an error needs to be set here */
      ssh_buffer_free(packet->payload);
      SAFE_FREE(packet);
      sftp_leave_function();
      return NULL;
    }
    if(buffer_add_data(packet->payload,buffer,r)==SSH_ERROR){
      ssh_buffer_free(packet->payload);
      SAFE_FREE(packet);
      sftp_leave_function();
      ssh_set_error_oom(sftp->session);
      return NULL;
    }
    size -= r;
  }

  sftp_leave_function();
  return packet;
}