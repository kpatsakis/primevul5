uint32_t ssh_buffer_pass_bytes(struct ssh_buffer_struct *buffer, uint32_t len){
    buffer_verify(buffer);

    if (buffer->pos + len < len || buffer->used < buffer->pos + len) {
        return 0;
    }

    buffer->pos+=len;
    /* if the buffer is empty after having passed the whole bytes into it, we can clean it */
    if(buffer->pos==buffer->used){
        buffer->pos=0;
        buffer->used=0;
    }
    buffer_verify(buffer);
    return len;
}