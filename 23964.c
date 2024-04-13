int sftp_server_init(sftp_session sftp){
  ssh_session session = sftp->session;
  sftp_packet packet = NULL;
  ssh_buffer reply = NULL;
  uint32_t version;

  sftp_enter_function();

  packet = sftp_packet_read(sftp);
  if (packet == NULL) {
    sftp_leave_function();
    return -1;
  }

  if (packet->type != SSH_FXP_INIT) {
    ssh_set_error(session, SSH_FATAL,
        "Packet read of type %d instead of SSH_FXP_INIT",
        packet->type);

    sftp_packet_free(packet);
    sftp_leave_function();
    return -1;
  }

  ssh_log(session, SSH_LOG_PACKET, "Received SSH_FXP_INIT");

  buffer_get_u32(packet->payload, &version);
  version = ntohl(version);
  ssh_log(session, SSH_LOG_PACKET, "Client version: %d", version);
  sftp->client_version = version;

  sftp_packet_free(packet);

  reply = ssh_buffer_new();
  if (reply == NULL) {
    ssh_set_error_oom(session);
    sftp_leave_function();
    return -1;
  }

  if (buffer_add_u32(reply, ntohl(LIBSFTP_VERSION)) < 0) {
    ssh_set_error_oom(session);
    ssh_buffer_free(reply);
    sftp_leave_function();
    return -1;
  }

  if (sftp_packet_write(sftp, SSH_FXP_VERSION, reply) < 0) {
    ssh_buffer_free(reply);
    sftp_leave_function();
    return -1;
  }
  ssh_buffer_free(reply);

  ssh_log(session, SSH_LOG_RARE, "Server version sent");

  if (version > LIBSFTP_VERSION) {
    sftp->version = LIBSFTP_VERSION;
  } else {
    sftp->version=version;
  }

  sftp_leave_function();
  return 0;
}