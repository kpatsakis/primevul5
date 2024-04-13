int sftp_init(sftp_session sftp) {
  sftp_packet packet = NULL;
  ssh_buffer buffer = NULL;
  ssh_string ext_name_s = NULL;
  ssh_string ext_data_s = NULL;
  char *ext_name = NULL;
  char *ext_data = NULL;
  uint32_t version = htonl(LIBSFTP_VERSION);

  sftp_enter_function();

  buffer = ssh_buffer_new();
  if (buffer == NULL) {
    ssh_set_error_oom(sftp->session);
    sftp_leave_function();
    return -1;
  }

  if (buffer_add_u32(buffer, version) < 0) {
    ssh_set_error_oom(sftp->session);
    ssh_buffer_free(buffer);
    sftp_leave_function();
    return -1;
  }
  if (sftp_packet_write(sftp, SSH_FXP_INIT, buffer) < 0) {
    ssh_buffer_free(buffer);
    sftp_leave_function();
    return -1;
  }
  ssh_buffer_free(buffer);

  packet = sftp_packet_read(sftp);
  if (packet == NULL) {
    sftp_leave_function();
    return -1;
  }

  if (packet->type != SSH_FXP_VERSION) {
    ssh_set_error(sftp->session, SSH_FATAL,
        "Received a %d messages instead of SSH_FXP_VERSION", packet->type);
    sftp_packet_free(packet);
    sftp_leave_function();
    return -1;
  }

  /* TODO: are we sure there are 4 bytes ready? */
  buffer_get_u32(packet->payload, &version);
  version = ntohl(version);
  ssh_log(sftp->session, SSH_LOG_RARE,
      "SFTP server version %d",
      version);

  ext_name_s = buffer_get_ssh_string(packet->payload);
  while (ext_name_s != NULL) {
    int count = sftp->ext->count;
    char **tmp;

    ext_data_s = buffer_get_ssh_string(packet->payload);
    if (ext_data_s == NULL) {
      ssh_string_free(ext_name_s);
      break;
    }

    ext_name = ssh_string_to_char(ext_name_s);
    ext_data = ssh_string_to_char(ext_data_s);
    if (ext_name == NULL || ext_data == NULL) {
      ssh_set_error_oom(sftp->session);
      SAFE_FREE(ext_name);
      SAFE_FREE(ext_data);
      ssh_string_free(ext_name_s);
      ssh_string_free(ext_data_s);
      return -1;
    }
    ssh_log(sftp->session, SSH_LOG_RARE,
        "SFTP server extension: %s, version: %s",
        ext_name, ext_data);

    count++;
    tmp = realloc(sftp->ext->name, count * sizeof(char *));
    if (tmp == NULL) {
      ssh_set_error_oom(sftp->session);
      SAFE_FREE(ext_name);
      SAFE_FREE(ext_data);
      ssh_string_free(ext_name_s);
      ssh_string_free(ext_data_s);
      return -1;
    }
    tmp[count - 1] = ext_name;
    sftp->ext->name = tmp;

    tmp = realloc(sftp->ext->data, count * sizeof(char *));
    if (tmp == NULL) {
      ssh_set_error_oom(sftp->session);
      SAFE_FREE(ext_name);
      SAFE_FREE(ext_data);
      ssh_string_free(ext_name_s);
      ssh_string_free(ext_data_s);
      return -1;
    }
    tmp[count - 1] = ext_data;
    sftp->ext->data = tmp;

    sftp->ext->count = count;

    ssh_string_free(ext_name_s);
    ssh_string_free(ext_data_s);

    ext_name_s = buffer_get_ssh_string(packet->payload);
  }

  sftp_packet_free(packet);

  sftp->version = sftp->server_version = version;

  sftp_leave_function();

  return 0;
}