static sftp_request_queue request_queue_new(sftp_message msg) {
  sftp_request_queue queue = NULL;

  queue = malloc(sizeof(struct sftp_request_queue_struct));
  if (queue == NULL) {
    ssh_set_error_oom(msg->sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(queue);

  queue->message = msg;

  return queue;
}