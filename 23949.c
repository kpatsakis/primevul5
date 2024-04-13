static sftp_message sftp_dequeue(sftp_session sftp, uint32_t id){
  sftp_request_queue prev = NULL;
  sftp_request_queue queue;
  sftp_message msg;

  if(sftp->queue == NULL) {
    return NULL;
  }

  queue = sftp->queue;
  while (queue) {
    if(queue->message->id == id) {
      /* remove from queue */
      if (prev == NULL) {
        sftp->queue = queue->next;
      } else {
        prev->next = queue->next;
      }
      msg = queue->message;
      request_queue_free(queue);
      ssh_log(sftp->session, SSH_LOG_PACKET,
          "Dequeued msg id %d type %d",
          msg->id,
          msg->packet_type);
      return msg;
    }
    prev = queue;
    queue = queue->next;
  }

  return NULL;
}