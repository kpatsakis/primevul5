uint16_t purge_extraneous_characters(char *data) {
  uint16_t purgings = 0;
  uint16_t str_len = strlen(data);
  for (uint16_t str_offset = 0; str_offset < str_len; str_offset++) {
    if (data[str_offset] < 32 || data[str_offset] == 127) {
      data[str_offset] = 19;
      purgings++;
      break;
    }
  }
  return purgings;
}