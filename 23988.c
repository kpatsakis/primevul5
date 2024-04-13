static sftp_message sftp_get_message(sftp_packet packet) {
  sftp_session sftp = packet->sftp;
  sftp_message msg = NULL;

  sftp_enter_function();

  msg = sftp_message_new(sftp);
  if (msg == NULL) {
    sftp_leave_function();
    return NULL;
  }

  msg->sftp = packet->sftp;
  msg->packet_type = packet->type;

  if ((packet->type != SSH_FXP_STATUS) && (packet->type!=SSH_FXP_HANDLE) &&
      (packet->type != SSH_FXP_DATA) && (packet->type != SSH_FXP_ATTRS) &&
      (packet->type != SSH_FXP_NAME) && (packet->type != SSH_FXP_EXTENDED_REPLY)) {
    ssh_set_error(packet->sftp->session, SSH_FATAL,
        "Unknown packet type %d", packet->type);
    sftp_message_free(msg);
    sftp_leave_function();
    return NULL;
  }

  if (buffer_get_u32(packet->payload, &msg->id) != sizeof(uint32_t)) {
    ssh_set_error(packet->sftp->session, SSH_FATAL,
        "Invalid packet %d: no ID", packet->type);
    sftp_message_free(msg);
    sftp_leave_function();
    return NULL;
  }

  ssh_log(packet->sftp->session, SSH_LOG_PACKET,
      "Packet with id %d type %d",
      msg->id,
      msg->packet_type);

  if (buffer_add_data(msg->payload, buffer_get_rest(packet->payload),
        buffer_get_rest_len(packet->payload)) < 0) {
    ssh_set_error_oom(sftp->session);
    sftp_message_free(msg);
    sftp_leave_function();
    return NULL;
  }

  sftp_leave_function();
  return msg;
}