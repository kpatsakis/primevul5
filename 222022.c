int ssh_buffer_add_ssh_string(struct ssh_buffer_struct *buffer,
    struct ssh_string_struct *string) {
  uint32_t len = 0;

  if (string == NULL) {
      return -1;
  }

  len = ssh_string_len(string);
  if (ssh_buffer_add_data(buffer, string, len + sizeof(uint32_t)) < 0) {
    return -1;
  }

  return 0;
}