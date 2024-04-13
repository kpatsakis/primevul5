static void sftp_message_free(sftp_message msg) {
  sftp_session sftp;

  if (msg == NULL) {
    return;
  }

  sftp = msg->sftp;
  sftp_enter_function();

  ssh_buffer_free(msg->payload);
  SAFE_FREE(msg);

  sftp_leave_function();
}