static sftp_status_message parse_status_msg(sftp_message msg){
  sftp_status_message status;

  if (msg->packet_type != SSH_FXP_STATUS) {
    ssh_set_error(msg->sftp->session, SSH_FATAL,
        "Not a ssh_fxp_status message passed in!");
    return NULL;
  }

  status = malloc(sizeof(struct sftp_status_message_struct));
  if (status == NULL) {
    ssh_set_error_oom(msg->sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(status);

  status->id = msg->id;
  if (buffer_get_u32(msg->payload,&status->status) != 4){
    SAFE_FREE(status);
    ssh_set_error(msg->sftp->session, SSH_FATAL,
        "Invalid SSH_FXP_STATUS message");
    return NULL;
  }
  status->error = buffer_get_ssh_string(msg->payload);
  status->lang = buffer_get_ssh_string(msg->payload);
  if(status->error == NULL || status->lang == NULL){
    if(msg->sftp->version >=3){
      /* These are mandatory from version 3 */
      ssh_string_free(status->error);
    /* status->lang never get allocated if something failed */
      SAFE_FREE(status);
      ssh_set_error(msg->sftp->session, SSH_FATAL,
        "Invalid SSH_FXP_STATUS message");
      return NULL;
    }
  }

  status->status = ntohl(status->status);
  if(status->error)
    status->errormsg = ssh_string_to_char(status->error);
  else
    status->errormsg = strdup("No error message in packet");
  if(status->lang)
    status->langmsg = ssh_string_to_char(status->lang);
  else
    status->langmsg = strdup("");
  if (status->errormsg == NULL || status->langmsg == NULL) {
    ssh_set_error_oom(msg->sftp->session);
    status_msg_free(status);
    return NULL;
  }

  return status;
}