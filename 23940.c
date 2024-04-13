static int sftp_enqueue(sftp_session sftp, sftp_message msg) {
  sftp_request_queue queue = NULL;
  sftp_request_queue ptr;

  queue = request_queue_new(msg);
  if (queue == NULL) {
    return -1;
  }

  ssh_log(sftp->session, SSH_LOG_PACKET,
      "Queued msg type %d id %d",
      msg->id, msg->packet_type);

  if(sftp->queue == NULL) {
    sftp->queue = queue;
  } else {
    ptr = sftp->queue;
    while(ptr->next) {
      ptr=ptr->next; /* find end of linked list */
    }
    ptr->next = queue; /* add it on bottom */
  }

  return 0;
}