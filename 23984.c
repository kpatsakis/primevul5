sftp_file sftp_open(sftp_session sftp, const char *file, int flags,
    mode_t mode) {
  sftp_message msg = NULL;
  sftp_status_message status;
  struct sftp_attributes_struct attr;
  sftp_file handle;
  ssh_string filename;
  ssh_buffer buffer;
  uint32_t sftp_flags = 0;
  uint32_t id;

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }

  filename = ssh_string_from_char(file);
  if (filename == NULL) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }

  ZERO_STRUCT(attr);
  attr.permissions = mode;
  attr.flags = SSH_FILEXFER_ATTR_PERMISSIONS;

  if (flags == O_RDONLY)
    sftp_flags |= SSH_FXF_READ; /* if any of the other flag is set,
                                   READ should not be set initialy */
  if (flags & O_WRONLY)
    sftp_flags |= SSH_FXF_WRITE;
  if (flags & O_RDWR)
    sftp_flags |= (SSH_FXF_WRITE | SSH_FXF_READ);
  if (flags & O_CREAT)
    sftp_flags |= SSH_FXF_CREAT;
  if (flags & O_TRUNC)
    sftp_flags |= SSH_FXF_TRUNC;
  if (flags & O_EXCL)
    sftp_flags |= SSH_FXF_EXCL;
  ssh_log(sftp->session,SSH_LOG_PACKET,"Opening file %s with sftp flags %x",file,sftp_flags);
  id = sftp_get_new_id(sftp);
  if (buffer_add_u32(buffer, id) < 0 ||
      buffer_add_ssh_string(buffer, filename) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    ssh_string_free(filename);
    return NULL;
  }
  ssh_string_free(filename);

  if (buffer_add_u32(buffer, htonl(sftp_flags)) < 0 ||
      buffer_add_attributes(buffer, &attr) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    return NULL;
  }
  if (sftp_packet_write(sftp, SSH_FXP_OPEN, buffer) < 0) {
    ssh_buffer_free(buffer);
    return NULL;
  }
  ssh_buffer_free(buffer);

  while (msg == NULL) {
    if (sftp_read_and_dispatch(sftp) < 0) {
      /* something nasty has happened */
      return NULL;
    }
    msg = sftp_dequeue(sftp, id);
  }

  switch (msg->packet_type) {
    case SSH_FXP_STATUS:
      status = parse_status_msg(msg);
      sftp_message_free(msg);
      if (status == NULL) {
        return NULL;
      }
      sftp_set_error(sftp, status->status);
      ssh_set_error(sftp->session, SSH_REQUEST_DENIED,
          "SFTP server: %s", status->errormsg);
      status_msg_free(status);

      return NULL;
    case SSH_FXP_HANDLE:
      handle = parse_handle_msg(msg);
      sftp_message_free(msg);
      return handle;
    default:
      ssh_set_error(sftp->session, SSH_FATAL,
          "Received message %d during open!", msg->packet_type);
      sftp_message_free(msg);
  }

  return NULL;
}