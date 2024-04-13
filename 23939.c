sftp_session sftp_server_new(ssh_session session, ssh_channel chan){
  sftp_session sftp = NULL;

  sftp = malloc(sizeof(struct sftp_session_struct));
  if (sftp == NULL) {
    ssh_set_error_oom(session);
    return NULL;
  }
  ZERO_STRUCTP(sftp);

  sftp->session = session;
  sftp->channel = chan;

  return sftp;
}