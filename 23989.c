static int sftp_read_and_dispatch(sftp_session sftp) {
  sftp_packet packet = NULL;
  sftp_message msg = NULL;

  sftp_enter_function();

  packet = sftp_packet_read(sftp);
  if (packet == NULL) {
    sftp_leave_function();
    return -1; /* something nasty happened reading the packet */
  }

  msg = sftp_get_message(packet);
  sftp_packet_free(packet);
  if (msg == NULL) {
    sftp_leave_function();
    return -1;
  }

  if (sftp_enqueue(sftp, msg) < 0) {
    sftp_message_free(msg);
    sftp_leave_function();
    return -1;
  }

  sftp_leave_function();
  return 0;
}