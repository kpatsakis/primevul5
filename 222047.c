int ssh_buffer_prepend_data(struct ssh_buffer_struct *buffer, const void *data,
    uint32_t len) {
  buffer_verify(buffer);

  if(len <= buffer->pos){
    /* It's possible to insert data between begin and pos */
    memcpy(buffer->data + (buffer->pos - len), data, len);
    buffer->pos -= len;
    buffer_verify(buffer);
    return 0;
  }
  /* pos isn't high enough */
  if (buffer->used - buffer->pos + len < len) {
    return -1;
  }

  if (buffer->allocated < (buffer->used - buffer->pos + len)) {
    if (realloc_buffer(buffer, buffer->used - buffer->pos + len) < 0) {
      return -1;
    }
  }
  memmove(buffer->data + len, buffer->data + buffer->pos, buffer->used - buffer->pos);
  memcpy(buffer->data, data, len);
  buffer->used += len - buffer->pos;
  buffer->pos = 0;
  buffer_verify(buffer);
  return 0;
}