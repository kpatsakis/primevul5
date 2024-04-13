void sftp_free(sftp_session sftp){
  sftp_request_queue ptr;

  if (sftp == NULL) {
    return;
  }

  ssh_channel_send_eof(sftp->channel);
  ptr = sftp->queue;
  while(ptr) {
    sftp_request_queue old;
    sftp_message_free(ptr->message);
    old = ptr->next;
    SAFE_FREE(ptr);
    ptr = old;
  }

  ssh_channel_free(sftp->channel);
  sftp_ext_free(sftp->ext);
  ZERO_STRUCTP(sftp);

  SAFE_FREE(sftp);
}