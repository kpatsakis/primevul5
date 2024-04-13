static void request_queue_free(sftp_request_queue queue) {
  if (queue == NULL) {
    return;
  }

  ZERO_STRUCTP(queue);
  SAFE_FREE(queue);
}