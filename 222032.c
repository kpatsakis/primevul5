void *ssh_buffer_get(struct ssh_buffer_struct *buffer){
    return buffer->data + buffer->pos;
}