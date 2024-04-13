static void status_msg_free(sftp_status_message status){
  if (status == NULL) {
    return;
  }

  ssh_string_free(status->error);
  ssh_string_free(status->lang);
  SAFE_FREE(status->errormsg);
  SAFE_FREE(status->langmsg);
  SAFE_FREE(status);
}