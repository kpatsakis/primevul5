static sftp_message sftp_message_new(sftp_session sftp){
  sftp_message msg = NULL;

  sftp_enter_function();

  msg = malloc(sizeof(struct sftp_message_struct));
  if (msg == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(msg);

  msg->payload = ssh_buffer_new();
  if (msg->payload == NULL) {
    ssh_set_error_oom(sftp->session);
    SAFE_FREE(msg);
    return NULL;
  }
  msg->sftp = sftp;

  sftp_leave_function();
  return msg;
}