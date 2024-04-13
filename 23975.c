int sftp_async_read(sftp_file file, void *data, uint32_t size, uint32_t id){
  sftp_session sftp = file->sftp;
  sftp_message msg = NULL;
  sftp_status_message status;
  ssh_string datastring;
  int err = SSH_OK;
  uint32_t len;

  sftp_enter_function();

  if (file->eof) {
    sftp_leave_function();
    return 0;
  }

  /* handle an existing request */
  while (msg == NULL) {
    if (file->nonblocking){
      if (ssh_channel_poll(sftp->channel, 0) == 0) {
        /* we cannot block */
        return SSH_AGAIN;
      }
    }

    if (sftp_read_and_dispatch(sftp) < 0) {
      /* something nasty has happened */
      sftp_leave_function();
      return SSH_ERROR;
    }

    msg = sftp_dequeue(sftp,id);
  }

  switch (msg->packet_type) {
    case SSH_FXP_STATUS:
      status = parse_status_msg(msg);
      sftp_message_free(msg);
      if (status == NULL) {
        sftp_leave_function();
        return -1;
      }
      sftp_set_error(sftp, status->status);
      if (status->status != SSH_FX_EOF) {
        ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
            "SFTP server : %s", status->errormsg);
        sftp_leave_function();
        err = SSH_ERROR;
      } else {
        file->eof = 1;
      }
      status_msg_free(status);
      sftp_leave_function();
      return err;
    case SSH_FXP_DATA:
      datastring = buffer_get_ssh_string(msg->payload);
      sftp_message_free(msg);
      if (datastring == NULL) {
        ssh_set_error(sftp->session, SSH_FATAL,
            "Received invalid DATA packet from sftp server");
        sftp_leave_function();
        return SSH_ERROR;
      }
      if (ssh_string_len(datastring) > size) {
        ssh_set_error(sftp->session, SSH_FATAL,
            "Received a too big DATA packet from sftp server: "
            "%" PRIdS " and asked for %u",
            ssh_string_len(datastring), size);
        ssh_string_free(datastring);
        sftp_leave_function();
        return SSH_ERROR;
      }
      len = ssh_string_len(datastring);
      /* Update the offset with the correct value */
      file->offset = file->offset - (size - len);
      memcpy(data, ssh_string_data(datastring), len);
      ssh_string_free(datastring);
      sftp_leave_function();
      return len;
    default:
      ssh_set_error(sftp->session,SSH_FATAL,"Received message %d during read!",msg->packet_type);
      sftp_message_free(msg);
      sftp_leave_function();
      return SSH_ERROR;
  }

  sftp_leave_function();
  return SSH_ERROR;
}