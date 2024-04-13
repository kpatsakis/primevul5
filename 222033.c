uint32_t ssh_buffer_get_len(struct ssh_buffer_struct *buffer){
  buffer_verify(buffer);
  return buffer->used - buffer->pos;
}