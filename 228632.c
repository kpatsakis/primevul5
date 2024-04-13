void mem_append(const char *add_string, char *dest_string) {
  uint8_t str_len = strlen(dest_string);
  if (str_len > 0) {
    memcpy(dest_string + str_len, ", ", 2);
    memcpy(dest_string + str_len + 2, add_string, strlen(add_string));
  } else {
    memcpy(dest_string, add_string, strlen(add_string));
  }
  return;
}