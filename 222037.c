uint32_t ssh_buffer_pass_bytes_end(struct ssh_buffer_struct *buffer, uint32_t len){
  buffer_verify(buffer);

  if (buffer->used < len) {
      return 0;
  }

  buffer->used-=len;
  buffer_verify(buffer);
  return len;
}