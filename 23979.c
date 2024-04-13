sftp_session sftp_new(ssh_session session){
  sftp_session sftp;

  if (session == NULL) {
    return NULL;
  }
  enter_function();

  sftp = malloc(sizeof(struct sftp_session_struct));
  if (sftp == NULL) {
    ssh_set_error_oom(session);
    leave_function();
    return NULL;
  }
  ZERO_STRUCTP(sftp);

  sftp->ext = sftp_ext_new();
  if (sftp->ext == NULL) {
    ssh_set_error_oom(session);
    SAFE_FREE(sftp);
    leave_function();
    return NULL;
  }

  sftp->session = session;
  sftp->channel = ssh_channel_new(session);
  if (sftp->channel == NULL) {
    SAFE_FREE(sftp);
    leave_function();
    return NULL;
  }

  if (ssh_channel_open_session(sftp->channel)) {
    ssh_channel_free(sftp->channel);
    SAFE_FREE(sftp);
    leave_function();
    return NULL;
  }

  if (ssh_channel_request_sftp(sftp->channel)) {
    sftp_free(sftp);
    leave_function();
    return NULL;
  }

  leave_function();
  return sftp;
}